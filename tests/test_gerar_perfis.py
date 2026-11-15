import unittest

from scripts.gerar_perfis import gerar_perfis


class TestGerarPerfis(unittest.TestCase):
    def test_perfis_tem_dois_rotulos_divisao_e_reprodutibilidade(self):
        primeiro = gerar_perfis(100, 0.10)
        segundo = gerar_perfis(100, 0.10)

        self.assertTrue(primeiro.equals(segundo))
        self.assertEqual(len(primeiro), 100)
        self.assertEqual(set(primeiro["divisao"]), {"treino", "teste"})
        self.assertEqual((primeiro["divisao"] == "treino").sum(), 70)
        self.assertTrue(primeiro["pes"].str.startswith("PES_").all())
        self.assertTrue(primeiro["ciclo"].str.startswith("CICLO_").all())

    def test_sem_ruido_respeita_alternativa_tipica(self):
        dados = gerar_perfis(100, 0)

        for linha in dados.to_dict(orient="records"):
            if linha["pes"] == "PES_IOT" and linha["ciclo"] == "CICLO_CC":
                self.assertEqual(linha["Q1"], "a")
                self.assertEqual(linha["Q3"], "d")

    def test_rejeita_ruido_invalido(self):
        with self.assertRaises(ValueError):
            gerar_perfis(10, 1.1)


if __name__ == "__main__":
    unittest.main()
