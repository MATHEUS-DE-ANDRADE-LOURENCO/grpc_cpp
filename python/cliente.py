# cliente.py (esqueleto)
import grpc
import notas_pb2
import notas_pb2_grpc

def run():
    with grpc.insecure_channel('localhost:50051') as channel:
        stub = notas_pb2_grpc.GerenciadorNotasStub(channel)



        ra = "123"
        # Adicionando várias notas para diferentes disciplinas
        disciplinas_notas = [
            ("CIC0001", 9.5),
            ("CIC0002", 8.7),
            ("CIC0003", 7.8),
            ("CIC0004", 6.9)
        ]
        for cod_disciplina, nota in disciplinas_notas:
            response = stub.AdicionarNota(notas_pb2.AdicionaNotaRequest(
                ra=ra,
                cod_disciplina=cod_disciplina,
                ano=2025,
                semestre=2,
                nota=nota
            ))
            print(f"Resposta do servidor (AdicionarNota {cod_disciplina}): {response.msg}")

        # Alterando uma nota
        response = stub.AlterarNota(notas_pb2.AdicionaNotaRequest(
            ra=ra,
            cod_disciplina="CIC0001",
            ano=2025,
            semestre=2,
            nota=8.0
        ))
        print(f"Resposta do servidor (AlterarNota CIC0001): {response.msg}")

        # Consultando uma nota específica
        consulta_response = stub.ConsultarNota(notas_pb2.AlunoDisciplinaRequest(
            ra=ra,
            cod_disciplina="CIC0001"
        ))
        if consulta_response.sucesso:
            nota = consulta_response.nota
            print(f"Nota consultada: RA={nota.ra}, Disciplina={nota.cod_disciplina}, Ano={nota.ano}, Semestre={nota.semestre}, Nota={nota.nota}")
        else:
            print(f"Erro ao consultar nota: {consulta_response.msg_erro}")

        # Calcular média
        media_response = stub.CalcularMedia(notas_pb2.AlunoRequest(ra=ra))
        if media_response.sucesso:
            print(f"Média do aluno {ra}: {media_response.media}")
        else:
            print(f"Erro ao calcular média: {media_response.msg_erro}")

        # --- CHAMADA DO DESAFIO (STREAMING) ---
        # TODO: Descomente este bloco de código após implementar o desafio 
        # no servidor.
        # O objetivo é iterar sobre o stream de respostas recebido do 
        # servidor e imprimir cada nota.
        
        print("\n--- Listando todas as notas do aluno 123 (DESAFIO) ---")
        try:
            notas_stream = stub.ListarNotasAluno(notas_pb2.AlunoRequest(ra="123"))
            for nota in notas_stream:
                print(f"  - Disciplina: {nota.cod_disciplina}, Nota: {nota.nota}")
        except grpc.RpcError as e:
            print(f"Erro ao chamar ListarNotasAluno: {e.details()}")
        

if __name__ == '__main__':
    run()