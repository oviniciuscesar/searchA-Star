#include "m_pd.h" //importa as funcões prontas do pd
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "string.h"


#define MAX_LINHA 256

//define uma nova classe
static t_class *searchA_class;


// Declaração antecipada da estrutura t_no
struct _no;

// //lista de arestas
typedef struct _aresta {
    struct _no *destino;  // Uso do tipo declarado antecipadamente
    float heuristic1;
    float heuristic2;
    float heuristic3;
    struct _aresta *next;
} t_aresta;



// //lista de nós
typedef struct _no {
    float heuristic1;
    float heuristic2;
    float heuristic3;
    int position;  // Armazena a posição do nó no arquivo (alterado 22/10/24)
    t_aresta *adjacentes;
    struct _no *next;
} t_no;


//esturtura do objeto
typedef struct _searchA {
    t_object x_obj;
    t_no *current_node;
    t_float custo_atual;
    t_float x_menor_custo;
    t_float custo_total;
    t_float heuristic_in[3];
    t_float weighting[3];
    t_no *grafo; //ponteiro para a estrutura t_no
    t_atom *x_nodeout;//armazena o nó atual para ser enviado no outlet
    t_atom *x_cost;
    t_symbol *name_mode;
    t_canvas *x_canvas;
    t_outlet *outlet1;//outlet 1
    t_outlet *outlet2;//outler 2
    t_outlet *outlet3;//outler 2
} t_searchA;



static void liberar_grafo(t_no *grafo) {
    while (grafo != NULL) {
        t_no *temp_no = grafo;
        t_aresta *adj = grafo->adjacentes;
        while (adj != NULL) {
            t_aresta *temp_adj = adj;
            adj = adj->next;
            freebytes(temp_adj, sizeof(t_aresta));
        }
        grafo = grafo->next;
        freebytes(temp_no, sizeof(t_no));
    }
}



void searchA_pond(t_searchA *x, t_symbol *s, int argc, t_atom *argv){
    // Recebe lista com 3 valores e armazena no array heuristic_in

    int i, j, k;
    if (argc == 3) {
        for (i = 0; i < argc; i++) {
            x->weighting[i] = argv[i].a_w.w_float;
            post("pesos: %0.2f", x->weighting[i]);
        }
    }
    if(x->name_mode == gensym("<")){
        for(j=0; j < argc; j++){
            if(x->weighting[j] == 2){
                x->weighting[j] = 0.5;
            }else if(x->weighting[j] == 1){
                x->weighting[j] = x->weighting[j];
            }
            post("pesos <: %0.2f", x->weighting[j]);
        } 
    }  
    if(x->name_mode == gensym(">")){
        for(k=0; k < argc; k++){
            if(x->weighting[k] == 0.5){
                x->weighting[k] = 2;
            }else if(x->weighting[k] == 1){
                x->weighting[k] = 1;
            }
            post("pesos >: %0.2f", x->weighting[k]);
        }
    }
}



//--------------------------------    configura o mode da busca (nó com menor custo ou maior custo) --------------------------------
void set_mode(t_searchA *x, t_symbol *symbol){
    x->name_mode = symbol;
    int k, j;
    
    // Compara o conteúdo da string utilizando strcmp
    if (strcmp(x->name_mode->s_name, "<") == 0) {
        for(j=0; j < 3; j++){
            if(x->weighting[j] == 2){
                x->weighting[j] = 0.5;
            }else if(x->weighting[j] == 1){
                x->weighting[j] = x->weighting[j];
            }
            post("pesos <: %0.2f", x->weighting[j]);
        } 
        post("mode: lowest cost");
    }
    else{
        // Compara o conteúdo da string utilizando strcmp
    if (strcmp(x->name_mode->s_name, ">") == 0) {
        for(k=0; k < 3; k++){
            if(x->weighting[k] == 0.5){
                x->weighting[k] = 2;
            }else if(x->weighting[k] == 1){
                x->weighting[k] = 1;
            }
            post("pesos >: %0.2f", x->weighting[k]);
        }
        post("mode: higher cost");
    }
    else{
        error("invalid argument type" );
    }
        
    }
    
}




//---------------------------------------- lê o grafo armazenado em um arquivo de texto ------------------------------------------
void searchA_read(t_searchA *x, t_symbol *filename, t_symbol *format) {
    int filedesc, num_nos = 0, line_number = 1;
    FILE *fd;
    char buf[MAXPDSTRING], *bufptr;
    char line[256]; // Buffer para ler uma linha do arquivo
    float heuristic1, heuristic2, heuristic3;

    if ((filedesc = open_via_path(canvas_getdir(x->x_canvas)->s_name,
        filename->s_name, "", buf, &bufptr, MAXPDSTRING, 0)) < 0) {
        error("%s: can't open", filename->s_name);
        return;
    }

    fd = fdopen(filedesc, "r");

    // Libera memória do grafo antigo, se necessário
    liberar_grafo(x->grafo);

    t_no *grafo = NULL;
    t_no *ultimo_no = NULL;

    // Leitura do arquivo linha por linha
    while (fgets(line, sizeof(line), fd)) {
        // Verificar se a linha é vazia ou contém apenas espaços
        if (line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        // Remove o caractere de nova linha (se houver)
        line[strcspn(line, "\n")] = 0;

        // Separa a parte antes do "->" e depois dele
        char *parte_nos = strtok(line, "->");
        char *parte_arestas = strtok(NULL, ";");

        // Lê o nó (os três primeiros valores antes do "->")
        if (sscanf(parte_nos, "%f %f %f", &heuristic1, &heuristic2, &heuristic3) == 3) {
            t_no *novo_no = (t_no *)getbytes(sizeof(t_no));
            if (novo_no == NULL) {
                error("Falha ao alocar memória para novo nó.");
                fclose(fd);
                return;
            }

            // Inicializa o nó com os valores lidos
            novo_no->heuristic1 = heuristic1;
            novo_no->heuristic2 = heuristic2;
            novo_no->heuristic3 = heuristic3;
            novo_no->adjacentes = NULL;
            novo_no->next = NULL;

            // Armazena a posição do nó (número da linha)
            novo_no->position = line_number;

            post("Node read: heuristic1 = %f, heuristic2 = %f, heuristic3 = %f, , position = %d",
                 novo_no->heuristic1, novo_no->heuristic2, novo_no->heuristic3, novo_no->position);

            // Incrementa o contador de nós
            num_nos++;
            line_number++;

            // Adiciona o nó ao grafo
            if (grafo == NULL) {
                grafo = novo_no;
            } else {
                ultimo_no->next = novo_no;
            }
            ultimo_no = novo_no;

            // Se houver arestas para esse nó, processa
            if (parte_arestas != NULL) {
                // post("Parte das arestas encontrada: %s", parte_arestas);

                // Remover o símbolo '>' do início da string de arestas, se estiver presente
                if (parte_arestas[0] == '>') {
                    parte_arestas++; // Pular o primeiro caractere
                }

                t_aresta *ultima_aresta = NULL;
                char *arestas_ptr;
                char *arestas_context;

                arestas_ptr = strtok_r(parte_arestas, " ", &arestas_context);
                while (arestas_ptr != NULL) {
                    if (sscanf(arestas_ptr, "%f", &heuristic1) == 1) {
                        arestas_ptr = strtok_r(NULL, " ", &arestas_context);
                        if (arestas_ptr != NULL && sscanf(arestas_ptr, "%f", &heuristic2) == 1) {
                            arestas_ptr = strtok_r(NULL, " ", &arestas_context);
                            if (arestas_ptr != NULL && sscanf(arestas_ptr, "%f", &heuristic3) == 1) {
                                t_aresta *nova_aresta = (t_aresta *)getbytes(sizeof(t_aresta));
                                if (nova_aresta == NULL) {
                                    error("Failed to allocate memory for new edge");
                                    fclose(fd);
                                    return;
                                }

                                // Inicializa a aresta com os valores lidos
                                nova_aresta->heuristic1 = heuristic1;
                                nova_aresta->heuristic2 = heuristic2;
                                nova_aresta->heuristic3 = heuristic3;
                                nova_aresta->next = NULL;

                                post("Eadge read: heuristic1 = %f, heuristic2 = %f, heuristic3 = %f",
                                     nova_aresta->heuristic1, nova_aresta->heuristic2, nova_aresta->heuristic3);

                                // Adiciona a aresta à lista de adjacentes do nó
                                if (novo_no->adjacentes == NULL) {
                                    novo_no->adjacentes = nova_aresta;
                                } else {
                                    ultima_aresta->next = nova_aresta;
                                }
                                ultima_aresta = nova_aresta;
                            } else {
                                error("Invalid edge format (third value).");
                                break;
                            }
                        } else {
                            error("Invalid edge format (second value)");
                            break;
                        }
                    } else {
                        error("Invalid edge format (firts value).");
                        break;
                    }

                    // Continua para a próxima aresta
                    arestas_ptr = strtok_r(NULL, " ", &arestas_context);
                }
            } else {
                post("No edges found for this node.");
            }
        } else {
            error("Invalid node format: %s", line);
        }
    }

    if (fclose(fd) != 0) {
        error("Error closing file.");
    }

    x->grafo = grafo;
    post("Graph loaded with %d nodes", num_nos);
}





//----------------------------------- realiza a busca no grafo  carregado --------------------------------------
static void searchA_bang(t_searchA *x, t_symbol *s, int argc, t_atom *argv) {
    int i, j, node_position = 0;  // Adiciona um contador para armazenar a posição do nó
    float custo_total, menor_custo, heuristica, custo_entre_nos;
    t_aresta *melhor_aresta = NULL;

    // Verifica se o grafo está vazio
    if (x->grafo == NULL) {
        error("Empty graph");
        return;
    }

    // Inicializa o nó atual como o primeiro nó do grafo, se ainda não foi inicializado
    if (x->current_node == NULL) {
        x->current_node = x->grafo;
        x->custo_atual = 0.0; // Inicializa o custo atual
    }

    // Recebe lista com 3 valores e armazena no array heuristic_in
    if (argc == 3) {
        for (i = 0; i < argc; i++) {
            x->heuristic_in[i] = argv[i].a_w.w_float * x->weighting[i];
            post("Entrada: %0.2f", x->heuristic_in[i]);
        }

        // Verifica se o nó atual tem arestas adjacentes
        if (x->current_node->adjacentes != NULL) {
            // Inicializa o menor custo como infinito para comparação
            // menor_custo = INFINITY;
            if (x->name_mode == gensym("<")) {
                menor_custo = INFINITY;  // Procurando o menor custo
            } else if (x->name_mode == gensym(">")) {
                 menor_custo = -INFINITY;  // Procurando o maior custo
            }
            melhor_aresta = NULL;
            // post("nó inicial possui arestas");
        

            // Percorre as arestas adjacentes ao nó atual
            t_aresta *adjacente = x->current_node->adjacentes; //
            while (adjacente != NULL) {

                // Calcula a heurística entre o nó atual e seus nós adjacentes (distância euclidiana)
                heuristica = sqrt(pow(x->heuristic_in[0] - adjacente->heuristic1, 2) +
                                       pow(x->heuristic_in[1] - adjacente->heuristic2, 2) +
                                       pow(x->heuristic_in[2] - adjacente->heuristic3, 2));
                // post("heuristica: %0.3f", heuristica);

                // Calcula o custo de um nó a outro (média entre heuristic1, heuristic2, heuristic3)
                custo_entre_nos = (adjacente->heuristic1 + adjacente->heuristic2 + adjacente->heuristic3) /3;
                // post("custo entre nós: %0.3f", custo_entre_nos);


                // Calcula o custo total: custo atual + custo entre nós adjacentes + heurística
                custo_total =  custo_entre_nos + heuristica + x->custo_atual;
                x->custo_total = custo_total;
                // post("custo total: %0.3f", custo_total);
                


                // Verifica se o custo total é o menor encontrado até agora
                if (x->name_mode == gensym("<")) {
                    if (custo_total < menor_custo) {
                    menor_custo = custo_total;
                    melhor_aresta = adjacente; 
                    // post("modo menor");   
                    }
                } else if (x->name_mode == gensym(">")) {
                    if (custo_total > menor_custo) {
                    menor_custo = custo_total;
                    melhor_aresta = adjacente; 
                    // post("modo maior");   
                    }
                }
                 adjacente = adjacente->next;
            }

            // post("menor custo: %0.3f", menor_custo);


            // Se uma aresta com menor custo total foi encontrada
            if (melhor_aresta != NULL) {
                // post("Aresta com menor custo total encontrada.");

                // Agora precisamos encontrar o nó correspondente no grafo
                t_no *no_atual = x->grafo;

                while (no_atual != NULL) {
                    if (no_atual->heuristic1 == melhor_aresta->heuristic1 &&
                        no_atual->heuristic2 == melhor_aresta->heuristic2 &&
                        no_atual->heuristic3 == melhor_aresta->heuristic3) {
                        // post("Novo nó encontrado no grafo:");
                        post("current node found: %0.3f, %0.3f, %0.3f",
                             no_atual->heuristic1, no_atual->heuristic2, no_atual->heuristic3);

                        // Atualiza o nó atual para o nó encontrado no grafo
                        x->current_node = no_atual;
                        x->custo_atual = menor_custo; // Atualiza o custo atual
                        break;
                    }
                    no_atual = no_atual->next; // Próximo nó no grafo
                }


                //prepara os dados e envia para o primeiro outlet (3 valores de heurística e a posição do nó no grafo)
                x->x_nodeout[0].a_type = A_FLOAT;
                x->x_nodeout[1].a_type = A_FLOAT;
                x->x_nodeout[2].a_type = A_FLOAT;
                x->x_nodeout[3].a_type = A_FLOAT; // Adiciona um quarto valor para a posição

                //define os valores no formato adequado para serem enviados ao otlet
                SETFLOAT(x->x_nodeout + 0, x->current_node->heuristic1);
                SETFLOAT(x->x_nodeout + 1, x->current_node->heuristic2); 
                SETFLOAT(x->x_nodeout + 2, x->current_node->heuristic3);
                SETFLOAT(x->x_nodeout + 3, x->current_node->position);

                outlet_list(x->outlet1, &s_list, 4, x->x_nodeout); // Envia os valores do nó atual e sua posição no grafo para o outlet

                
                // post("total: %0.3f", x->custo_total);
                outlet_float(x->outlet2, x->custo_total);
            

                // Se nenhum nó correspondente foi encontrado
                if (no_atual == NULL) {
                    error("No node found");
                }
            } else {
                error("No conections found.");
            }
        } else {
            outlet_bang(x->outlet3);
            post("Last node reached or current node dosn`t have valid conections.");
        }
    } else {
        error("Please, provide a list with three values");
    }
}

//TODO: setar argumentos junto com o objeto


//----------------------------- reenicia a busca ------------------------------
void searchA_restart(t_searchA *x){
    x->current_node = x->grafo;
    x->custo_atual = 0.0; // Inicializa o custo atual
    post("Graph restarted");
}



static void *searchA_new(t_symbol *s){ //argc é a quantidade de elementos da lista e argv é um ponteiro para uma lista
    t_searchA *x = (t_searchA *)pd_new(searchA_class);

    x->outlet1 = outlet_new(&x->x_obj, &s_list);
    x->outlet2 = outlet_new(&x->x_obj, &s_float);
    x->outlet3 = outlet_new(&x->x_obj, &s_anything);
    x->x_canvas = canvas_getcurrent();
    x->x_nodeout = (t_atom *)getbytes(4 * sizeof(t_atom));
    x->name_mode = gensym("<");
    x->weighting[0] = 1;
    x->weighting[1] = 1;
    x->weighting[2] = 1;
    post("searchA v0.01");
    return (x);

}



void searchA_destroy(t_searchA *x){

    outlet_free(x->outlet1);
    outlet_free(x->outlet2);
    outlet_free(x->outlet3);
    liberar_grafo(x->grafo);
    freebytes(x->x_nodeout, 4 * sizeof(t_atom));
    freebytes(x->x_cost, 1 * sizeof(t_atom));
    // freebytes(x->current_node, 3 * sizeof(t_float)); não precisa desalocar memória

}



void searchA_setup(void) {
	searchA_class = class_new(
		gensym("searchA"), //nome do objeto
		(t_newmethod)searchA_new, //chama a função construtor
		(t_method)searchA_destroy, //chama a função destruidor
		sizeof(t_searchA),
        CLASS_DEFAULT,
         A_DEFFLOAT, 0);//tamanho do objeto

        //  class_addlist(searchA*_class, (t_method) searchA*_list);
        class_addmethod(searchA_class, (t_method)searchA_read, gensym("read"), A_SYMBOL, 0);
        class_addlist(searchA_class, (t_method) searchA_bang);
        class_addmethod(searchA_class, (t_method)searchA_restart, gensym("restart"), A_GIMME, 0);
        class_addmethod(searchA_class, (t_method)set_mode, gensym("mode"), A_SYMBOL, 0);
        class_addmethod(searchA_class, (t_method)searchA_pond, gensym("weighting"), A_GIMME, 0);
    }