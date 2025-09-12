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

        grpc::gerencia_notas::Nota nova_nota;
        nova_nota.set_ra(request->ra());
        nova_nota.set_cod_disciplina(request->cod_disciplina());
        nova_nota.set_nota(request->nota());
        db_notas_cpp[chave] = nova_nota;

        response->set_sucesso(true);
        response->set_msg("Nota adicionada com sucesso.");
        return grpc::Status::OK;
    }

    grpc::Status AlterarNota(grpc::ServerContext* context,
                               const gerencia_notas::AdicionaNotaRequest* request,
                               gerencia_notas::StatusResponse* response) override {

        std::cout << "Recebido pedido para alterar a nota do RA "
                  << request->ra() << " na disciplina " << request->cod_disciplina() << std::endl;
                  
        std::string chave = request->ra() + "_" + request->cod_disciplina();

        

        if (!db_notas_cpp.count(chave)) {
            response->set_sucesso(false);
            response->set_msg("Nota não encontrada.");
            return grpc::Status::OK;
        }

        db_notas_cpp[chave].set_nota(request->nota());


        
        response->set_sucesso(true);
        response->set_msg("Nota alterada com sucesso.");
        return grpc::Status::OK;
    }

    grpc::Status consultarNota(grpc::ServerContext* context,
                               const gerencia_notas::AlunoDisciplinaRequest* request,
                               gerencia_notas::ConsultaNotaResponse* response) override {

        std::cout << "Recebido pedido para consultar a nota do RA "
                  << request->ra() << " na disciplina " << request->cod_disciplina() << std::endl;
                  
        std::string chave = request->ra() + "_" + request->cod_disciplina();

        

        if (!db_notas_cpp.count(chave)) {
            response->set_sucesso(false);
            response->set_msg_erro("Nota não encontrada.");
            return grpc::Status::OK;
        }

        response->set_nota(db_notas_cpp[chave]);
        response->set_sucesso(true);
        response->set_msg("Nota: " + std::to_string(db_notas_cpp[chave].nota()));

        return grpc::Status::OK;
    }

    grpc::Status CalcularMedia(grpc::ServerContext* context,
                               const gerencia_notas::AlunoRequest* request,
                               gerencia_notas::MediaResponse* response) override {

        std::cout << "Recebido pedido para calcular a média do RA "
                  << request->ra() << std::endl;
                  
        double soma = 0.0;
        int count = 0;

        for (const auto& par : db_notas_cpp) {
            const gerencia_notas::Nota& nota = par.second;
            if (nota.ra() == request->ra()) {
                soma += nota.nota();
                count++;
            }
        }

        if (count == 0) {
            response->set_sucesso(false);
            response->set_msg_erro("Nenhuma nota encontrada para o RA.");
            return grpc::Status::OK;
        }

        double media = soma / count;
        response->set_media(media);
        response->set_sucesso(true);
        response->set_msg("Média calculada com sucesso.");

        return grpc::Status::OK;
    }

    grpc::Status ListarNotasAluno(grpc::ServerContext* context,
                                  const gerencia_notas::AlunoRequest* request,
                                  grpc::ServerWriter<gerencia_notas::Nota>* writer) override {
        std::cout << "Recebido pedido para listar notas do RA "
                  << request->ra() << std::endl;

        for (const auto& par : db_notas_cpp) {
            const gerencia_notas::Nota& nota = par.second;
            if (nota.ra() == request->ra()) {
                if (!writer->Write(nota)) {
                    std::cerr << "Erro ao enviar nota para o cliente." << std::endl;
                    return grpc::Status::OK;
                }
            }
        }

        return grpc::Status::OK;
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
