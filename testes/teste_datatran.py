"""Confere o leitor da Parte 2 contra o arquivo original, sem alterar seus bytes."""

import argparse
import csv
from pathlib import Path
import tempfile

from teste_programa import RAIZ, ESTRUTURAS, executar, exigir


def rodar(executavel, entrada, consulta, saida, **opcoes):
    return executar(executavel, entrada, consulta, saida, leitor="datatran",
                    delimitador=";", codificacao="latin-1", colunas=30, **opcoes)


def numero(linha, campo, cabecalho):
    return float(linha[cabecalho.index(campo)].replace(",", "."))


def nota(linha, cabecalho):
    km = max(0, 1 - abs(numero(linha, "km", cabecalho) - 150) / 200)
    pessoas = max(0, 1 - abs(numero(linha, "pessoas", cabecalho) - 3) / 3)
    return (km + pessoas) / 2


def organizar(linhas, estrutura, cabecalho):
    uf = cabecalho.index("uf")
    tipo = cabecalho.index("tipo_acidente")
    if estrutura == "pilha":
        return list(reversed(linhas))
    if estrutura == "lista_ord":
        return sorted(linhas, key=lambda r: nota(r, cabecalho), reverse=True)
    if estrutura == "lista_gen":
        grupos = {}
        for linha in linhas:
            grupos.setdefault(linha[uf], {}).setdefault(linha[tipo], []).append(linha)
        return [linha for grupo in grupos.values()
                for subgrupo in grupo.values() for linha in subgrupo]
    if estrutura == "lista_cruz":
        return sorted(linhas, key=lambda r: r[uf])
    return list(linhas)


def testar_matriz(executavel, entrada, consulta, saida, cabecalho, amostra):
    consulta.write_text(
        "FILTER|feridos|>=|1\nSIM|km|150|200|1\nSIM|pessoas|3|3|1\n"
        "ACCEPT|0.30\nORDER|similarity|DESC\n"
        "GROUP|uf|tipo_acidente\nCROSS|uf|km\n", encoding="utf-8",
    )
    for i, base in enumerate(ESTRUTURAS):
        for j, resultado in enumerate(ESTRUTURAS):
            top_k = 0 if (i + j) % 3 == 0 else 7
            metricas, linhas = rodar(executavel, entrada, consulta, saida, base=base,
                                    resultado=resultado, limite=len(amostra), top_k=top_k)
            candidatos = [r for r in organizar(amostra, base, cabecalho)
                          if numero(r, "feridos", cabecalho) >= 1 and nota(r, cabecalho) >= 0.30]
            esperados = organizar(candidatos, resultado, cabecalho)
            if top_k:
                esperados = esperados[:top_k]
            exigir(linhas == [cabecalho] + esperados, f"DATATRAN: ordem incorreta {base}/{resultado}")
            exigir(int(metricas["INPUT_RECORDS"]) == len(amostra), "DATATRAN: limite de carga")
            exigir(int(metricas["MATCHED_RECORDS"]) == len(candidatos), "DATATRAN: aceitos")
            exigir(int(metricas["OUTPUT_RECORDS"]) == len(esperados), "DATATRAN: top-k")


def testar_valores(executavel, entrada, consulta, saida, cabecalho, amostra):
    casos = [
        ("FILTER|km|<=|146.1\n", lambda r: numero(r, "km", cabecalho) <= 146.1),
        ("FILTER|latitude|<|-20.1\n", lambda r: numero(r, "latitude", cabecalho) < -20.1),
        ("FILTER|longitude|>=|-48.3\n", lambda r: numero(r, "longitude", cabecalho) >= -48.3),
        ("FILTER|classificacao_acidente|==|Com Vítimas Feridas\n",
         lambda r: r[cabecalho.index("classificacao_acidente")] == "Com Vítimas Feridas"),
        ("FILTER|tipo_acidente|==|Colisão frontal\n",
         lambda r: r[cabecalho.index("tipo_acidente")] == "Colisão frontal"),
        ("FILTER|tracado_via|==|Aclive;Reta\n",
         lambda r: r[cabecalho.index("tracado_via")] == "Aclive;Reta"),
        ("FILTER|classificacao_acidente|==|NA\n",
         lambda r: r[cabecalho.index("classificacao_acidente")] == "NA"),
    ]
    for texto, predicado in casos:
        consulta.write_text(texto, encoding="utf-8")
        _, linhas = rodar(executavel, entrada, consulta, saida, limite=len(amostra), top_k=0)
        exigir(linhas[1:] == [r for r in amostra if predicado(r)], f"DATATRAN: {texto}")
    for direcao in ("ASC", "DESC"):
        consulta.write_text(f"ORDER|km|{direcao}\n", encoding="utf-8")
        _, linhas = rodar(executavel, entrada, consulta, saida, resultado="lista_ord",
                         limite=len(amostra), top_k=0)
        esperados = sorted(amostra, key=lambda r: numero(r, "km", cabecalho),
                           reverse=direcao == "DESC")
        exigir(linhas[1:] == esperados, "DATATRAN: ORDER precisa comparar decimais")
    consulta.write_text("CROSS|km|uf\n", encoding="utf-8")
    _, linhas = rodar(executavel, entrada, consulta, saida, resultado="lista_cruz",
                     limite=len(amostra), top_k=0)
    exigir(linhas[1:] == sorted(amostra, key=lambda r: numero(r, "km", cabecalho)),
           "DATATRAN: eixo numerico da lista cruzada")


def testar_consultas_entregues(executavel, entrada, saida, cabecalho, registros):
    for nome in ("datatran_graves.txt", "datatran_proximidade.txt"):
        resultado = "lista_gen" if "graves" in nome else "lista_ord"
        metricas, linhas = rodar(executavel, entrada, RAIZ / "consultas" / nome,
                                saida, resultado=resultado, limite=0, top_k=20)
        exigir(int(metricas["OUTPUT_RECORDS"]) > 0, "consulta entregue precisa produzir resultados")
        exigir(int(metricas["INPUT_RECORDS"]) == len(registros), "consulta na base inteira")
        if "graves" in nome:
            esperado = sum(numero(r, "mortos", cabecalho) >= 1 for r in registros)
        else:
            def aceito(r):
                km = max(0, 1 - abs(numero(r, "km", cabecalho) - 150) / 50)
                pessoas = max(0, 1 - abs(numero(r, "pessoas", cabecalho) - 3) / 3)
                return (r[cabecalho.index("uf")] == "MG" and
                        numero(r, "feridos", cabecalho) >= 1 and (km + pessoas) / 2 >= 0.40)
            esperado = sum(aceito(r) for r in registros)
        exigir(int(metricas["MATCHED_RECORDS"]) == esperado, "contagem da consulta entregue")


def main():
    argumentos = argparse.ArgumentParser()
    argumentos.add_argument("--executavel", type=Path, default=RAIZ / "trabalho")
    argumentos.add_argument("--sem-base-completa", action="store_true")
    opcoes = argumentos.parse_args()
    entrada = RAIZ / "datatran2026.csv"
    original = entrada.read_bytes()
    with entrada.open(newline="", encoding="latin-1") as arquivo:
        linhas = list(csv.reader(arquivo, delimiter=";"))
    cabecalho, registros = linhas[0], linhas[1:]
    with tempfile.TemporaryDirectory(prefix="trabalho-datatran-") as temporario:
        consulta = Path(temporario) / "consulta.txt"
        saida = Path(temporario) / "saida.csv"
        testar_matriz(opcoes.executavel, entrada, consulta, saida, cabecalho, registros[:150])
        testar_valores(opcoes.executavel, entrada, consulta, saida, cabecalho, registros[:150])
        consulta.write_text("", encoding="utf-8")
        rodar(opcoes.executavel, RAIZ / "asteroid_dataset_20251019.csv", consulta, saida, sucesso=False)
        executar(opcoes.executavel, entrada, consulta, saida, sucesso=False)
        if not opcoes.sem_base_completa:
            metricas, _ = rodar(opcoes.executavel, entrada, consulta, saida, limite=0, top_k=0)
            exigir(saida.read_bytes() == original, "DATATRAN: preservar bytes, Latin-1, aspas e delimitador")
            exigir(int(metricas["OUTPUT_RECORDS"]) == len(registros), "DATATRAN: exportacao integral")
            testar_consultas_entregues(opcoes.executavel, entrada, saida, cabecalho, registros)
    exigir(entrada.read_bytes() == original, "DATATRAN: dataset original foi alterado")
    print("DATATRAN: 64 combinacoes, acentos, decimais, consultas e formato original verificados.")


if __name__ == "__main__":
    main()
