#include <iostream>
#include <string>
#include <grpcpp/grpcpp.h>
#include "notas.grpc.pb.h"
#include <map>

std::map<std::string, gerencia_notas::Nota> db_notas_cpp;

class GerenciadorNotasImpl final : public gerencia_notas::GerenciadorNotas::Service {
public:
    grpc::Status AdicionarNota(grpc::ServerContext* context,
                               const gerencia_notas::AdicionaNotaRequest* request,
                               gerencia_notas::StatusResponse* response) override {
        std::cout << "Recebido pedido para adicionar nota para RA "
                  << request->ra() << std::endl;

        std::string chave = request->ra() + "_" + request->cod_disciplina();

        if (db_notas_cpp.count(chave)) {
            response->set_sucesso(false);
            response->set_msg("Nota já existe.");
            return grpc::Status::OK;
        }

        // Lógica para criar e armazenar a nota...
        response->set_sucesso(true);
        response->set_msg("Nota adicionada com sucesso.");
        return grpc::Status::OK;
    }

    grpc::Status ListarNotasAluno(grpc::ServerContext* context,
                                  const gerencia_notas::AlunoRequest* request,
                                  grpc::ServerWriter<gerencia_notas::Nota>* writer) override {
        std::cout << "Recebido pedido para listar notas do RA "
                  << request->ra() << " (NÃO IMPLEMENTADO)" << std::endl;

        return grpc::Status(grpc::StatusCode::UNIMPLEMENTED,
                            "Funcionalidade de streaming não implementada.");
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50052");
    GerenciadorNotasImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Servidor C++ escutando em " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
