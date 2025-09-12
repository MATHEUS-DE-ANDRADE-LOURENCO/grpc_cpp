#include <iostream>
#include <string>
#include <grpcpp/grpcpp.h>
#include "notas.grpc.pb.h"

class NotasClient {
public:
    NotasClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(gerencia_notas::GerenciadorNotas::NewStub(channel)) {}

    void AdicionarNotaExemplo() {
        gerencia_notas::AdicionaNotaRequest request;
        request.set_ra("456");
        request.set_cod_disciplina("CIC0002");
        request.set_nota(8.0);

        gerencia_notas::StatusResponse reply;
        grpc::ClientContext context;

        grpc::Status status = stub_->AdicionarNota(&context, request, &reply);

        if (status.ok()) {
            std::cout << "Resposta do servidor: " << reply.msg() << std::endl;
        } else {
            std::cout << "Erro RPC: " << status.error_code()
                      << ": " << status.error_message() << std::endl;
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
    NotasClient client(grpc::CreateChannel("localhost:50051",
                                           grpc::InsecureChannelCredentials()));
    client.AdicionarNotaExemplo();

    // client.ListarNotasExemplo("123");

    return 0;
}
