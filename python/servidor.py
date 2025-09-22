# servidor.py

import grpc
from concurrent import futures

import notas_pb2
import notas_pb2_grpc

# =====================================
# Armazenamento em memória (simulando um banco de dados)
# =====================================
# Exemplo: { "123_DISC1": Nota(...), "123_DISC2": Nota(...) }
db_notas = {}

# =====================================
# Implementação do serviço GerenciadorNotas
# =====================================
class GerenciadorNotasServicer(notas_pb2_grpc.GerenciadorNotasServicer):

    def AdicionarNota(self, request, context):
        print(f"Adicionando nota para RA {request.ra} na disciplina {request.cod_disciplina}")

        chave = f"{request.ra}_{request.cod_disciplina}"

        if chave in db_notas:
            return notas_pb2.StatusResponse(
                sucesso=False,
                msg="Nota já existe para este aluno/disciplina. Use AlterarNota."
            )

        # Criação da nota
        nota = notas_pb2.Nota(
            ra=request.ra,
            cod_disciplina=request.cod_disciplina,
            ano=request.ano,
            semestre=request.semestre,
            nota=request.nota
        )

        db_notas[chave] = nota

        return notas_pb2.StatusResponse(
            sucesso=True,
            msg="Nota adicionada com sucesso!"
        )

    # ----------------------------
    # Outros métodos (a implementar)
    # ----------------------------

    def AlterarNota(self, request, context):
        print(f"Alterando a nota do aluno {request.ra} na disciplina {request.cod_disciplina}")

        chave = f"{request.ra}_{request.cod_disciplina}"

        # Se a nota a ser alterada não existir, retornar erro.
        if chave not in db_notas:
            return notas_pb2.StatusResponse(
                sucesso=False,
                msg='ERRO: Nota não existe para alteração.'
            )

        nota = notas_pb2.Nota(
            ra=request.ra,
            cod_disciplina=request.cod_disciplina,
            ano=request.ano,
            semestre=request.semestre,
            nota=request.nota
        )

        db_notas[chave] = nota

        return notas_pb2.StatusResponse(
            sucesso=True,
            msg='Nota alterada com sucesso.'
        )

    def ConsultarNota(self, request, context):
        chave = f"{request.ra}_{request.cod_disciplina}"

        if chave not in db_notas:
            return notas_pb2.ConsultaNotaResponse(
                sucesso=False,
                nota=None,
                msg_erro="Nota não está presente na lista"
            )

        nota = db_notas[chave]

        print(f"Foi realizada uma consulta de nota do aluno {request.ra} na disciplina {request.cod_disciplina}.")
        return notas_pb2.ConsultaNotaResponse(
            sucesso=True,
            nota=nota,
            msg_erro=""
        )

    def CalcularMedia(self, request, context):
        ra = request.ra
        soma = 0
        count = 0
        for nota in db_notas.values():
            if nota.ra == ra:
                count += 1
                soma += nota.nota
        if count == 0:
            return notas_pb2.MediaResponse(
                sucesso=False,
                media=-1,
                msg_erro="Não foram encontradas notas para esse RA"
            )

        print(f"Foi calculada a média do aluno {request.ra}.")
        return notas_pb2.MediaResponse(
            sucesso=True,
            media=soma/count,
            msg_erro=""
        )

    # ----------------------------
    # Desafio: Server-Side Streaming
    # ----------------------------
    def ListarNotasAluno(self, request, context):
        """
        Envia todas as notas associadas ao RA do aluno usando streaming.
        """
        print(f"Listando notas para o RA {request.ra}")

        encontrou = False
        for chave, nota in db_notas.items():
            if nota.ra == request.ra:
                encontrou = True
                yield nota

        if not encontrou:
            context.set_code(grpc.StatusCode.NOT_FOUND)
            context.set_details("Nenhuma nota encontrada para este aluno.")

# =====================================
# Função principal para iniciar o servidor
# =====================================
def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    notas_pb2_grpc.add_GerenciadorNotasServicer_to_server(
        GerenciadorNotasServicer(), server
    )

    server.add_insecure_port('[::]:50051')
    server.start()
    print("Servidor gRPC rodando na porta 50051.")
    server.wait_for_termination()

# =====================================
# Ponto de entrada
# =====================================
if __name__ == '__main__':
    serve()
