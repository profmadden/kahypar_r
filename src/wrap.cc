#include <memory>
#include <vector>
#include <iostream>
#include <thread>

#include <libkahypar.h>
static kahypar_context_t *context = NULL;


#if __has_include("disabled-libmtkahypar.h")
#define HAS_MT 1
#include <libmtkahypar.h>
static mt_kahypar_context_t *mtcontext = NULL;
#else
#define HAS_MT 0
#endif

#define LDBG 0


int test_partitioner() {

  // kahypar_context_t* context = kahypar_context_new();
  context = kahypar_context_new();
  kahypar_configure_context_from_file(context, "/usr/local/etc/kahypar.ini");

  kahypar_set_seed(context, 999);
  const kahypar_hypernode_weight_t hnw = 4;
  const kahypar_hypernode_id_t num_vertices = 7;
  const kahypar_hyperedge_id_t num_hyperedges = 4;

  std::unique_ptr<kahypar_hyperedge_weight_t[]> hyperedge_weights = std::make_unique<kahypar_hyperedge_weight_t[]>(4);

  // force the cut to contain hyperedge 0 and 2
  hyperedge_weights[0] = 1;
  hyperedge_weights[1] = 1000;
  hyperedge_weights[2] = 1;
  hyperedge_weights[3] = 1000;

  std::unique_ptr<size_t[]> hyperedge_indices = std::make_unique<size_t[]>(5);

  hyperedge_indices[0] = 0;
  hyperedge_indices[1] = 2;
  hyperedge_indices[2] = 6;
  hyperedge_indices[3] = 9;
  hyperedge_indices[4] = 12;

  std::unique_ptr<kahypar_hyperedge_id_t[]> hyperedges = std::make_unique<kahypar_hyperedge_id_t[]>(12);

  // hypergraph from hMetis manual page 14
  hyperedges[0] = 0;
  hyperedges[1] = 2;
  hyperedges[2] = 0;
  hyperedges[3] = 1;
  hyperedges[4] = 3;
  hyperedges[5] = 4;
  hyperedges[6] = 3;
  hyperedges[7] = 4;
  hyperedges[8] = 6;
  hyperedges[9] = 2;
  hyperedges[10] = 5;
  hyperedges[11] = 6;

  const double imbalance = 0.03;
  const kahypar_partition_id_t k = 2;

  kahypar_hyperedge_weight_t objective = 0;

  std::vector<kahypar_partition_id_t> partition(num_vertices, -1);

  kahypar_partition(num_vertices, num_hyperedges,
                    imbalance, k,
                    /*vertex_weights */ nullptr, hyperedge_weights.get(),
                    hyperedge_indices.get(), hyperedges.get(),
                    &objective, context, partition.data());

  printf("Finished partitioner call\n");

  for (int i = 0; i != num_vertices; ++i)
  {
    std::cout << i << ":" << partition[i] << std::endl;
  }

  kahypar_context_free(context);
  context = NULL;
}


extern "C"
{
  void kahypar_hello()
  {
    printf("Testing the partitioner\n");
    test_partitioner();

#if HAS_MT    
    printf("Testing to see if we can initialize for MT KAHYPAR\n");
    mt_kahypar_initialize_thread_pool(std::thread::hardware_concurrency(), true);
    mt_kahypar_context_t *context = mt_kahypar_context_new();
    printf("Found the MT context\n");
#else
    printf("MT-KaHyPar not available.\n");
#endif
  }

  int evaluate(int num_edges, size_t *eind, kahypar_hyperedge_id_t *eptr, int *part)
  {
    int total = 0;
    for (int i = 0; i < num_edges; ++i)
    {
      int side0 = 0;
      int side1 = 0;
      for (int j = eind[i]; j < eind[i + 1]; ++j)
      {
        if (part[eptr[j]])
          side1 = 1;
        else
          side0 = 1;
      }
      if (side0 && side1)
        ++total;
    }
    return total;
  }


  void mtpartition(unsigned int nvtxs, unsigned int nhedges, int *hewt, int *vtw, size_t *eind, kahypar_hyperedge_id_t *eptr, int *part, int kway, int passes, long seed)
  {
 #if HAS_MT
    if (mtcontext == NULL)
    {
      printf("Loading in the MT-KAHYPAR context\n");
      mt_kahypar_initialize_thread_pool(std::thread::hardware_concurrency(), true);
      mtcontext = mt_kahypar_context_new();
      mt_kahypar_load_preset(mtcontext, DEFAULT);
      // mt_kahypar_preset_type_t *mtpreset = 
    }

    size_t highest = eind[nhedges];
    mt_kahypar_hyperedge_id_t eptr2[highest];
    for (int i = 0; i < highest; ++i)
      eptr2[i] = eptr[i];

    printf("MT %d vertices, %d hyperedges highest index %d\n", nvtxs, nhedges, (int) highest);
    mt_kahypar_set_partitioning_parameters(mtcontext, 2, 0.03, KM1);
    mt_kahypar_hypergraph_t hg = mt_kahypar_create_hypergraph(DEFAULT, nvtxs, nhedges, eind, eptr2, hewt, vtw);
    mt_kahypar_partitioned_hypergraph_t phg = mt_kahypar_partition(hg, mtcontext);
    mt_kahypar_partition_id_t result[mt_kahypar_num_hypernodes(hg)];
    mt_kahypar_get_partition(phg, result);
    for (int i = 0; i < 10 && i < nvtxs; ++i)
      printf("[%d]", result[i]);
    printf("\n");
    for (int i = 0; i < nvtxs; ++i)
      part[i] = result[i];
    // mt_kahypar_hypergraph_t hg = mt_kahypar_create_hypergraph(DEFAULT, nvtxs, nhedges, eind, eptr2, hewt, vtw);
#if 0    
    mt_kahypar_add_fixed_vertices(hg, part, nvtxs);


#endif
    mt_kahypar_free_hypergraph(hg);
    mt_kahypar_free_partitioned_hypergraph(phg);
    printf("Finished with the MT PARTITION call\n");
#else
  // kahypar_partition(nvtxs, nhedges, hewt, vtw, eind, eptr, part, kway, passes, seed);
  printf("Error!  MT KaHyPar not available!");
  exit(1);
#endif
  }

  
  
  void partition(unsigned int nvtxs, unsigned int nhedges, int *hewt, int *vtw, size_t *eind, kahypar_hyperedge_id_t *eptr, int *part, int kway, int passes, long seed, float imbalance)
  {
    if (context == NULL)
    {
      context = kahypar_context_new();
      kahypar_configure_context_from_file(context, "/usr/local/etc/kahypar.ini");
      // kahypar_set_seed(context, 999);
    }
    

    if (LDBG)
    {
      printf("Calling KAHYPAR %d vertices, %d edges\n", nvtxs, nhedges);
      for (int i = 0; i < nvtxs; ++i)
        printf("VTX %d weight %d  initial partition %d\n", i, vtw[i], part[i]);

      for (int i = 0; i < nhedges; ++i)
      {
        printf("eptr %d -- %d to %d\n", i, eind[i], eind[i + 1]);
        for (int j = eind[i]; j < eind[i + 1]; ++j)
        {
          printf(" %d  ", eptr[j]);
        }
        printf("\n");
      }

      printf("Context %p\n", context);
    }

    kahypar_hyperedge_weight_t objective = 0;
    // const double imbalance = 0.01;
    const kahypar_partition_id_t k = kway;

    // printf("---------------\n");

    kahypar_hypergraph_t *hg = kahypar_create_hypergraph(k, nvtxs, nhedges, eind, eptr, hewt, vtw);
#if 0    
    int newpart[nvtxs];
    for (int i = 0; i < nvtxs; ++i)
        newpart[i] = -1;
    newpart[0] = 1;
    newpart[1] = 0;
#endif
    // printf("Calling the HG version\n");

    kahypar_set_seed(context, seed);
    kahypar_set_fixed_vertices(hg, part);
    kahypar_partition_hypergraph(hg, k, (double) imbalance, &objective, context, part);
    
#if 0
    for (int i = 0; i < nvtxs; ++i)
      part[i] = newpart[i];
#endif
    kahypar_hypergraph_free(hg);

#if 0
    int bestpart = -1;
    int finalpart[nvtxs];

    for (int pass = 0; pass < 2; ++pass) {
      kahypar_set_seed(context, (pass + 1) * 8675);

      kahypar_partition(nvtxs, nhedges, imbalance, k, NULL, NULL, eind, eptr, &objective, context, newpart);
      int cut = evaluate(nhedges, eind, eptr, newpart);
      // printf("C calculated cut %d\n", cut);
      if ((bestpart < 0) || (cut < bestpart))
      {
        for (int i = 0; i < nvtxs; ++i)
          finalpart[i] = newpart[i];
        bestpart = cut;
      }
      if (LDBG)
      {
        printf("Done with partitioning.  Result:\n");
        for (int i = 0; i < nvtxs; ++i)
          printf("%d  --- %d\n", i, part[i]);
      }
    }
    for (int i = 0; i < nvtxs; ++i)
      part[i] = finalpart[i];
     
#endif     
  }

}
