"""Verifica o programa contra o CSV fornecido, sem gerar outro dataset."""

import argparse
import csv
import hashlib
import math
from pathlib import Path
import subprocess
import tempfile


RAIZ = Path(__file__).resolve().parents[1]
ESTRUTURAS = (
    "pilha", "fila", "lista_seq", "lista_enc", "lista_cabeca",
    "lista_ord", "lista_gen", "lista_cruz",
)
CHAVES = (
    "STATUS", "INPUT_RECORDS", "MATCHED_RECORDS", "OUTPUT_RECORDS",
    "LOAD_MS", "SEARCH_MS", "RESULT_INSERT_MS", "WRITE_MS",
)


def exigir(condicao, mensagem):
    if not condicao:
        raise AssertionError(mensagem)


def nota(linha):
    velocidade = max(0.0, 1 - abs(float(linha[6]) - 15) / 10)
    risco = max(0.0, 1 - abs(float(linha[11]) - 0.70) / 0.30)
    return (velocidade + 2 * risco) / 3


def organizar(linhas, estrutura):
    # Esta referencia em Python verifica a ordem; os TADs do programa continuam em C.
    if estrutura == "pilha":
        return list(reversed(linhas))
    if estrutura == "lista_ord":
        return sorted(linhas, key=nota, reverse=True)
    if estrutura == "lista_gen":
        grupos = {}
        for linha in linhas:
            grupos.setdefault(linha[13], {}).setdefault(linha[3], []).append(linha)
        return [linha for grupo in grupos.values()
                for subgrupo in grupo.values() for linha in subgrupo]
    if estrutura == "lista_cruz":
        return sorted(linhas, key=lambda linha: linha[13])
    return list(linhas)


def aceito(linha):
    return (linha[9].lower() == "true" and float(linha[5]) <= 0.08
            and nota(linha) >= 0.05)


def executar(executavel, entrada, consulta, saida, base="lista_seq",
             resultado="fila", limite=150, top_k=7, extras=(), sucesso=True,
             leitor="nasa", delimitador=",", codificacao="utf-8", colunas=20):
    comando = [str(executavel), "--reader", leitor, "--input", str(entrada),
               "--base", base, "--result", resultado, "--query", str(consulta),
               "--output", str(saida), "--limit", str(limite), "--top-k", str(top_k)]
    processo = subprocess.run(comando + list(extras), text=True, capture_output=True)
    exigir(not any(aviso in processo.stderr for aviso in (
        "AddressSanitizer", "LeakSanitizer", "runtime error:",
    )), f"erro de memoria ou comportamento indefinido:\n{processo.stderr}")
    exigir((processo.returncode == 0) == sucesso,
           f"codigo de retorno inesperado: {comando}\n{processo.stderr}")
    metricas = dict(linha.split("=", 1) for linha in processo.stdout.splitlines())
    exigir(all(chave in metricas for chave in CHAVES), "metricas ausentes")
    exigir(metricas["STATUS"] == ("OK" if sucesso else "ERROR"), "STATUS incorreto")
    for chave in CHAVES[4:]:
        valor = float(metricas[chave])
        exigir(math.isfinite(valor) and valor >= 0, f"tempo invalido: {chave}")
    if not sucesso:
        exigir(processo.stderr, "falha deve explicar o erro")
        return metricas, []
    with saida.open(newline="", encoding=codificacao) as arquivo:
        linhas = list(csv.reader(arquivo, delimiter=delimitador))
    exigir(len(linhas[0]) == colunas, "cabecalho deve conservar as colunas da entrada")
    return metricas, linhas


def testar_matriz(executavel, entrada, consulta, saida, cabecalho, amostra):
    consulta.write_text(
        "FILTER|is_past_event|==|true\nFILTER|distance_au|<=|0.08\n"
        "SIM|velocity_km_s|15.0|10.0|1.0\nSIM|risk_score|0.70|0.30|2.0\n"
        "ACCEPT|0.05\nORDER|similarity|DESC\n"
        "GROUP|threat_category|year\nCROSS|threat_category|year\n",
        encoding="utf-8",
    )
    exigir(any(aceito(linha) for linha in amostra), "amostra precisa conter aceitos")
    for i, base in enumerate(ESTRUTURAS):
        for j, resultado in enumerate(ESTRUTURAS):
            limite_saida = 0 if (i + j) % 3 == 0 else 7
            metricas, linhas = executar(executavel, entrada, consulta, saida, base,
                                       resultado, len(amostra), limite_saida)
            candidatos = [linha for linha in organizar(amostra, base) if aceito(linha)]
            esperados = organizar(candidatos, resultado)
            if limite_saida:
                esperados = esperados[:limite_saida]
            exigir(linhas[0] == cabecalho, "cabecalho alterado")
            exigir(linhas[1:] == esperados, f"resultado incorreto: {base}/{resultado}")
            exigir(int(metricas["INPUT_RECORDS"]) == len(amostra), "limite de carga")
            exigir(int(metricas["MATCHED_RECORDS"]) == len(candidatos), "aceitos")
            exigir(int(metricas["OUTPUT_RECORDS"]) == len(esperados), "top-k")


def testar_filtros(executavel, entrada, consulta, saida, amostra):
    casos = [
        ("FILTER|is_past_event|==|TRUE\nACCEPT|1\n", lambda r: r[9].lower() == "true"),
        ("FILTER|is_future_event|!=|true\n", lambda r: r[10].lower() != "true"),
        ("FILTER|distance_au|>|0.08\n", lambda r: float(r[5]) > 0.08),
        ("FILTER|distance_au|<|0.08\n", lambda r: float(r[5]) < 0.08),
        ("FILTER|risk_score|>=|0.2\n", lambda r: float(r[11]) >= 0.2),
        ("FILTER|threat_category|==|MONITOR\n", lambda r: r[13] == "MONITOR"),
        ("FILTER|sentry_impact_prob|==|\n", lambda r: r[16] == ""),
        ("FILTER|sentry_impact_prob|>=|0\n", lambda r: r[16] != "" and float(r[16]) >= 0),
    ]
    for texto, predicado in casos:
        consulta.write_text(texto, encoding="utf-8")
        metricas, linhas = executar(executavel, entrada, consulta, saida,
                                   limite=len(amostra), top_k=0)
        esperados = [linha for linha in amostra if predicado(linha)]
        exigir(linhas[1:] == esperados, f"filtro incorreto: {texto}")
        exigir(int(metricas["MATCHED_RECORDS"]) == len(esperados), "contagem do filtro")
    consulta.write_text("FILTER|year|<|0\n", encoding="utf-8")
    metricas, linhas = executar(executavel, entrada, consulta, saida)
    exigir(len(linhas) == 1 and metricas["OUTPUT_RECORDS"] == "0", "resultado vazio")
    for direcao in ("ASC", "DESC"):
        consulta.write_text(f"ORDER|distance_au|{direcao}\n", encoding="utf-8")
        _, linhas = executar(executavel, entrada, consulta, saida,
                             resultado="lista_ord", limite=len(amostra), top_k=0)
        esperados = sorted(amostra, key=lambda r: float(r[5]), reverse=direcao == "DESC")
        exigir(linhas[1:] == esperados, "ORDER numerico incorreto")
    consulta.write_text("SIM|sentry_impact_prob|0|1|1\nACCEPT|0.5\n", encoding="utf-8")
    _, linhas = executar(executavel, entrada, consulta, saida, limite=len(amostra), top_k=0)
    exigir(all(linha[16] != "" for linha in linhas[1:]), "SIM ausente nao pode ganhar nota")
    consulta.write_text("", encoding="utf-8")
    _, linhas = executar(executavel, entrada, consulta, saida, base="pilha",
                         resultado="lista_ord", limite=len(amostra), top_k=0)
    exigir(linhas[1:] == amostra, "sem ORDER a lista ordenada usa a posicao original")
    _, linhas = executar(executavel, entrada, consulta, saida, limite=1, top_k=1)
    exigir(linhas[1:] == amostra[:1], "limite unitario")


def testar_erros(executavel, entrada, consulta, saida):
    for texto in (
        "FILTER|inexistente|==|1\n", "FILTER|year|??|2020\n",
        "SIM|risk_score|1|0|1\n", "SIM|risk_score|1|1|-1\n",
        "SIM|risk_score|1|1|0\n", "SIM|risk_score|NaN|1|1\n",
        "ACCEPT|1.5\n", "ORDER|year|CRESCENTE\n", "COMANDO|year\n",
        "ACCEPT|0\nACCEPT|1\n", "GROUP|year|inexistente\n",
        "CROSS|year|inexistente\n",
    ):
        consulta.write_text(texto, encoding="utf-8")
        executar(executavel, entrada, consulta, saida, sucesso=False)
    consulta.write_text("", encoding="utf-8")
    for parametros in (
        {"base": "lista_gen"}, {"resultado": "lista_cruz"}, {"limite": -1},
        {"top_k": "9999999999999999999999999999999999999999"},
        {"extras": ("--limit", "2")}, {"extras": ("--desconhecido", "2")},
        {"leitor": "outro"}, {"entrada": entrada.with_name("ausente.csv")},
        {"saida": saida.parent / "inexistente" / "resultado.csv"},
        {"saida": entrada},
    ):
        argumentos = {"entrada": entrada, "consulta": consulta, "saida": saida}
        argumentos.update(parametros)
        executar(executavel, **argumentos, sucesso=False)


def main():
    argumentos = argparse.ArgumentParser()
    argumentos.add_argument("--executavel", type=Path, default=RAIZ / "trabalho")
    argumentos.add_argument("--sem-base-completa", action="store_true")
    opcoes = argumentos.parse_args()
    entrada = RAIZ / "asteroid_dataset_20251019.csv"
    original = entrada.read_bytes()
    with entrada.open(newline="", encoding="utf-8") as arquivo:
        leitor = csv.reader(arquivo)
        cabecalho = next(leitor)
        amostra = [next(leitor) for _ in range(150)]
    with tempfile.TemporaryDirectory(prefix="trabalho-alg-") as temporario:
        # Consultas e resultados de teste ficam fora da pasta do dataset original.
        pasta = Path(temporario)
        consulta = pasta / "consulta.txt"
        saida = pasta / "saida.csv"
        testar_matriz(opcoes.executavel, entrada, consulta, saida, cabecalho, amostra)
        testar_filtros(opcoes.executavel, entrada, consulta, saida, amostra)
        testar_erros(opcoes.executavel, entrada, consulta, saida)
        if not opcoes.sem_base_completa:
            consulta.write_text("", encoding="utf-8")
            metricas, _ = executar(opcoes.executavel, entrada, consulta, saida,
                                  limite=0, top_k=0)
            exigir(saida.read_bytes() == original, "ida e volta deve preservar o CSV inteiro")
            with entrada.open(newline="", encoding="utf-8") as arquivo:
                total = sum(1 for _ in csv.reader(arquivo)) - 1
            exigir(int(metricas["INPUT_RECORDS"]) == total, "carga de toda a base")
            exigir(int(metricas["OUTPUT_RECORDS"]) == total, "exportacao de toda a base")
    exigir(hashlib.sha256(entrada.read_bytes()).digest() == hashlib.sha256(original).digest(),
           "o dataset original deve permanecer intacto")
    print("Programa: 64 combinacoes, consultas, erros, metricas e preservacao do CSV verificados.")


if __name__ == "__main__":
    main()
