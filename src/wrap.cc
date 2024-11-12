#include <memory>
#include <vector>
#include <iostream>

#include <libkahypar.h>

#define LDBG 0

static kahypar_context_t *context = NULL;

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
  
  void partition(unsigned int nvtxs, unsigned int nhedges, int *hewt, int *vtw, size_t *eind, kahypar_hyperedge_id_t *eptr, int *part)
  {
    if (context == NULL)
    {
      context = kahypar_context_new();
      kahypar_configure_context_from_file(context, "/usr/local/etc/kahypar.ini");
      kahypar_set_seed(context, 999);
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
    const double imbalance = 0.01;
    const kahypar_partition_id_t k = 2;

    printf("---------------\n");

    int bestpart = -1;
    int finalpart[nvtxs];

    for (int pass = 0; pass < 2; ++pass) {
      kahypar_set_seed(context, (pass + 1) * 8675);
      int newpart[nvtxs];
      for (int i = 0; i < nvtxs; ++i)
        newpart[i] = part[i];
      kahypar_partition(nvtxs, nhedges, imbalance, k, NULL, NULL, eind, eptr, &objective, context, newpart);
      int cut = evaluate(nhedges, eind, eptr, newpart);
      printf("C calculated cut %d\n", cut);
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
  }
}
