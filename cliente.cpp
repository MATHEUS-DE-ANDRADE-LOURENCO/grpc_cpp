#include <iostream>
#include <string>
#include <grpcpp/grpcpp.h>
#include "notas.grpc.pb.h"

class NotasClient {
public:
    NotasClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(gerencia_notas::GerenciadorNotas::NewStub(channel)) {}


    void AdicionarNotaExemplo(const std::string& ra, const std::string& cod_disciplina, double nota) {
        gerencia_notas::AdicionaNotaRequest request;
        request.set_ra(ra);
        request.set_cod_disciplina(cod_disciplina);
        request.set_nota(nota);

        gerencia_notas::StatusResponse reply;
        grpc::ClientContext context;

        grpc::Status status = stub_->AdicionarNota(&context, request, &reply);

        std::cout << "[AdicionarNota] ";
        if (status.ok()) {
            std::cout << reply.msg() << std::endl;
        } else {
            std::cout << "Erro RPC: " << status.error_code()
                      << ": " << status.error_message() << std::endl;
        }
    }

    void AlterarNotaExemplo(const std::string& ra, const std::string& cod_disciplina, double nova_nota) {
        gerencia_notas::AdicionaNotaRequest request;
        request.set_ra(ra);
        request.set_cod_disciplina(cod_disciplina);
        request.set_nota(nova_nota);

        gerencia_notas::StatusResponse reply;
        grpc::ClientContext context;

        grpc::Status status = stub_->AlterarNota(&context, request, &reply);

        std::cout << "[AlterarNota] ";
        if (status.ok()) {
            std::cout << reply.msg() << std::endl;
        } else {
            std::cout << "Erro RPC: " << status.error_code()
                      << ": " << status.error_message() << std::endl;
        }
    }

    void ConsultarNotaExemplo(const std::string& ra, const std::string& cod_disciplina) {
        gerencia_notas::AlunoDisciplinaRequest request;
        request.set_ra(ra);
        request.set_cod_disciplina(cod_disciplina);

        gerencia_notas::ConsultaNotaResponse reply;
        grpc::ClientContext context;

        grpc::Status status = stub_->consultarNota(&context, request, &reply);

        std::cout << "[ConsultarNota] ";
        if (status.ok()) {
            if (reply.sucesso()) {
                std::cout << reply.msg() << std::endl;
            } else {
                std::cout << reply.msg_erro() << std::endl;
            }
        } else {
            std::cout << "Erro RPC: " << status.error_code()
                      << ": " << status.error_message() << std::endl;
        }
    }

    void CalcularMediaExemplo(const std::string& ra) {
        gerencia_notas::AlunoRequest request;
        request.set_ra(ra);

        gerencia_notas::MediaResponse reply;
        grpc::ClientContext context;

        grpc::Status status = stub_->CalcularMedia(&context, request, &reply);

        std::cout << "[CalcularMedia] ";
        if (status.ok()) {
            if (reply.sucesso()) {
                std::cout << "Média: " << reply.media() << std::endl;
            } else {
                std::cout << reply.msg_erro() << std::endl;
            }
        } else {
            std::cout << "Erro RPC: " << status.error_code()
                      << ": " << status.error_message() << std::endl;
        }
    }

    void ListarNotasAlunoExemplo(const std::string& ra) {
        gerencia_notas::AlunoRequest request;
        request.set_ra(ra);

        grpc::ClientContext context;
        std::unique_ptr<grpc::ClientReader<gerencia_notas::Nota>> reader(
            stub_->ListarNotasAluno(&context, request));
        gerencia_notas::Nota nota;
        std::cout << "[ListarNotasAluno] Notas do RA " << ra << ":\n";
        bool recebeu = false;
        while (reader->Read(&nota)) {
            recebeu = true;
            std::cout << "  Disciplina: " << nota.cod_disciplina()
                      << ", Nota: " << nota.nota() << std::endl;
        }
        grpc::Status status = reader->Finish();
        if (!recebeu) {
            std::cout << "  Nenhuma nota recebida ou funcionalidade não implementada." << std::endl;
        }
        if (!status.ok()) {
            std::cout << "  Status: " << status.error_message() << std::endl;
        }
    }

    /*
    void ListarNotasExemplo(const std::string& ra) {
        // TODO: implementar streaming
    }
    */

private:
    std::unique_ptr<gerencia_notas::GerenciadorNotas::Stub> stub_;
};

int main(int argc, char** argv) {
    NotasClient client(grpc::CreateChannel("localhost:50052",
                                           grpc::InsecureChannelCredentials()));

    std::string ra = "456";
    std::string cod_disciplina = "CIC0002";
    double nota_inicial = 8.0;
    double nota_alterada = 9.5;

    client.AdicionarNotaExemplo(ra, cod_disciplina, nota_inicial);
    client.AlterarNotaExemplo(ra, cod_disciplina, nota_alterada);
    client.ConsultarNotaExemplo(ra, cod_disciplina);
    client.CalcularMediaExemplo(ra);
    client.ListarNotasAlunoExemplo(ra);

    return 0;
}
