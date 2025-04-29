#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Cache.h"
#include "Debug.h"
#include "MemoryManager.h"

class ICache : public Cache {
  public:
    //Constructor
    ICache(MemoryManager *manager, Policy policy, Cache *lowerCache) : Cache(manager, policy, lowerCache){ }
    void setByte(uint32_t addr, uint8_t val, uint32_t *cycles = nullptr){return;}
  };
bool parseParameters(int argc, char **argv);
void printUsage();
void runSplit(std::ofstream &csvFile, uint32_t cacheSize,
                   uint32_t blockSize, uint32_t associativity);
void runUnified(std::ofstream &csvFile, uint32_t cacheSize,
                   uint32_t blockSize, uint32_t associativity);

bool verbose = false;
bool isSingleStep = false;
bool isSplit = false;
const char *traceFilePath;

int main(int argc, char **argv) {
  if (!parseParameters(argc, argv)) {
    return -1;
  }

  // Open CSV file and write header
  std::ofstream csvFile(std::string(traceFilePath) + ".csv");
  csvFile << "splitOrUnified,cacheSize,blockSize,associativity,missCount,totalCycles\n";

  runSplit(csvFile, 8 * 1024, 64, 1);
  runUnified(csvFile, 16 * 1024, 64, 1);

  printf("Result has been written to %s\n",
         (std::string(traceFilePath) + ".csv").c_str());
  csvFile.close();
  return 0;
}

bool parseParameters(int argc, char **argv) {
  // Read Parameters
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'v':
        verbose = 1;
        break;
      case 's':
        isSingleStep = 1;
        break;
      default:
        return false;
      }
    } else {
      if (traceFilePath == nullptr) {
        traceFilePath = argv[i];
      } else {
        return false;
      }
    }
  }
  if (traceFilePath == nullptr) {
    return false;
  }
  return true;
}

void printUsage() {
  printf("Usage: CacheSim trace-file [-s] [-v]\n");
  printf("Parameters: -s single step, -v verbose output\n");
}

Cache::Policy createSingleLevelPolicy(uint32_t cacheSize,
                                     uint32_t blockSize,
                                     uint32_t associativity) {
    Cache::Policy policy;
    policy.cacheSize = cacheSize;
    policy.blockSize = blockSize;
    policy.blockNum = cacheSize / blockSize;
    policy.associativity = associativity;
    policy.hitLatency = 1;
    policy.missLatency = 100;
    return policy;
}

void runSplit(std::ofstream &csvFile, uint32_t cacheSize,
                   uint32_t blockSize, uint32_t associativity) {
    auto policy = createSingleLevelPolicy(cacheSize, blockSize, associativity);

    MemoryManager *memory = nullptr;
    ICache *icache = nullptr;
    Cache *dcache = nullptr;
    memory = new MemoryManager();
    icache = new ICache(memory, policy, nullptr);
    dcache = new Cache(memory, policy, nullptr);

    icache->printInfo(false);
    dcache->printInfo(false);
    // Read and execute trace in cache-trace/ folder
    std::ifstream trace(traceFilePath);
    if (!trace.is_open()) {
      printf("Unable to open file %s\n", traceFilePath);
      exit(-1);
    }

    char op; //'r' for read, 'w' for write
    uint32_t addr;
    char type;
    while (trace >> op >> std::hex >> addr >> type) {
      if (verbose)
        printf("%c %x\n", op, addr);
      if (!memory->isPageExist(addr))
        memory->addPage(addr);
      switch (op) {
      case 'r':
        switch (type) {
        case 'I':
          icache->getByte(addr);
          break;
        case 'D':
          dcache->getByte(addr);
          break;
        default:
          dbgprintf("Illegal type %c\n", type);
          exit(-1);
        }
        break;
      case 'w':
        switch (type) {
        case 'I':
          icache->setByte(addr, 0);
          break;
        case 'D':
          dcache->setByte(addr, 0);
          break;
        default:
          dbgprintf("Illegal type %c\n", type);
          exit(-1);
        }
        break;
      default:
        dbgprintf("Illegal op %c\n", op);
        exit(-1);
      }

      if (verbose) {
        icache->printInfo(true);
        dcache->printInfo(true);
      }

      if (isSingleStep) {
        printf("Press Enter to Continue...");
        getchar();
      }
    }

    // Output Simulation Results
    icache->printStatistics();
    dcache->printStatistics();

    csvFile << "Split," << icache->statistics.numMiss + dcache->statistics.numMiss << ","
            << std::max(icache->statistics.totalCycles, dcache->statistics.totalCycles)
            << std::endl;

    delete icache;
    delete dcache;
    delete memory;
}

void runUnified(std::ofstream &csvFile, uint32_t cacheSize,
                   uint32_t blockSize, uint32_t associativity) {
    auto policy = createSingleLevelPolicy(cacheSize, blockSize, associativity);

    MemoryManager *memory = nullptr;
    Cache *cache = nullptr;
    memory = new MemoryManager();
    cache = new Cache(memory, policy, nullptr);
    cache->printInfo(false);

    // Read and execute trace in cache-trace/ folder
    std::ifstream trace(traceFilePath);
    if (!trace.is_open()) {
      printf("Unable to open file %s\n", traceFilePath);
      exit(-1);
    }

    char op; //'r' for read, 'w' for write
    uint32_t addr;
    char type;
    while (trace >> op >> std::hex >> addr >> type) {
      if (verbose)
        printf("%c %x\n", op, addr);
      if (!memory->isPageExist(addr))
        memory->addPage(addr);
      switch (op) {
      case 'r':
        cache->getByte(addr);
        break;
      case 'w':
        cache->setByte(addr, 0);
        break;
      default:
        dbgprintf("Illegal op %c\n", op);
        exit(-1);
      }

      if (verbose)
        cache->printInfo(true);

      if (isSingleStep) {
        printf("Press Enter to Continue...");
        getchar();
      }
    }

    // Output Simulation Results
    cache->printStatistics();

    csvFile << "Unified," << cache->statistics.numMiss
            << "," << cache->statistics.totalCycles << std::endl;

    delete cache;
    delete memory;
}