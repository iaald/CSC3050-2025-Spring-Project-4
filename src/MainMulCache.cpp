
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Cache.h"
#include "Debug.h"
#include "MemoryManager.h"
#include "MultiLevelCacheConfig.h"

bool parseParameters(int argc, char **argv);
void printUsage();

bool isPreFetching = false;
bool isFIFO = false;
bool isVictimCache = false;

bool ispf = false;
uint32_t lastAddr = 0;
int sameStrideCount = 1;
int differentStrideCount = 1;
uint32_t lastStride = 0;

const char *traceFilePath;

class CacheHierarchy {
private:
    MemoryManager* memory;
    Cache* l1cache;
    Cache* l2cache;
    Cache* l3cache;

public:
    CacheHierarchy() {
        memory = new MemoryManager();

        auto l1policy = MultiLevelCacheConfig::getL1Policy();
        auto l2policy = MultiLevelCacheConfig::getL2Policy();
        auto l3policy = MultiLevelCacheConfig::getL3Policy();
        auto victimpolicy = MultiLevelCacheConfig::getVictimPolicy();

        if(isFIFO){
            l1policy.associativity = 256;
        }
        l3cache = new Cache(memory, l3policy, nullptr);
        l2cache = new Cache(memory, l2policy, l3cache);
        l1cache = new Cache(memory, l1policy, l2cache);
        if(isFIFO){
            l1cache->isFIFO = true;
        }
        if(isVictimCache){
            l1cache->isVictimCache = true;
            l1cache->victimCache = new Cache(memory, victimpolicy, nullptr);
        }
        memory->setCache(l1cache);
    }

    ~CacheHierarchy() {
        delete l1cache;
        delete l2cache;
        delete l3cache;
        delete memory;
    }

    void processMemoryAccess(char op, uint32_t addr) {
        if (!memory->isPageExist(addr)) {
            memory->addPage(addr);
        }

        switch (op) {
            case 'r':
                l1cache->read(addr);
                break;
            case 'w':
                l1cache->write(addr, 0);
                break;
            default:
                throw std::runtime_error("Illegal memory access operation");
        }
        if (isPreFetching) {
            uint32_t currentStride = static_cast<int>(addr - lastAddr);
            if (currentStride == lastStride) {
                sameStrideCount ++;
                differentStrideCount = 1;
            } else {
                differentStrideCount++;
                sameStrideCount = 1;
            }
            lastStride = currentStride;
            if (!ispf && sameStrideCount > 3)
                ispf = true;
            if (ispf && differentStrideCount > 3)
                ispf = false;
            if (ispf) {
                uint32_t prefetchAddr = addr + lastStride;
                if (!memory->isPageExist(prefetchAddr)) {
                    memory->addPage(prefetchAddr);
                }
                l1cache->read(prefetchAddr, true);
            }
            lastAddr = addr;
        }
    }

    void printResults() const {
        printf("\n=== Cache Hierarchy Statistics ===\n");
        l1cache->printStatistics();
    }

    void outputResults() const {

        printResults();

        std::string csvPath = std::string(traceFilePath) + "_multi_level.csv";
        std::ofstream csvFile(csvPath);

        csvFile << "Level,NumReads,NumWrites,NumHits,NumMisses,MissRate,TotalCycles\n";

        outputCacheStats(csvFile, "L1");
        outputCacheStats(csvFile, "L2");
        outputCacheStats(csvFile, "L3");

        csvFile.close();
        printf("\nResults have been written to %s\n", csvPath.c_str());
    }
private:
    void outputCacheStats(std::ofstream& csvFile, const char* level) const {
        auto& stats = l1cache->statistics;
        switch(level[1]) {
        case '1':
            stats = l1cache->statistics;
            break;
        case '2':
            stats = l2cache->statistics;
            break;
        case '3':
            stats = l3cache->statistics;
            break;
        default:
            throw std::runtime_error("Invalid cache level");
        }
        float missRate = static_cast<float>(stats.numMiss) /
                        (stats.numHit + stats.numMiss) * 100;

        csvFile << level << ","
                << stats.numRead << ","
                << stats.numWrite << ","
                << stats.numHit << ","
                << stats.numMiss << ","
                << missRate << ","
                << stats.totalCycles << "\n";
    }
};

int main(int argc, char **argv) {
    if (!parseParameters(argc, argv)) {
        printUsage();
        return -1;
    }

    std::ifstream trace(traceFilePath);
    if (!trace.is_open()) {
        printf("Unable to open file %s\n", traceFilePath);
        return -1;
    }

    try {
        CacheHierarchy cacheHierarchy;
        char op;
        uint32_t addr;

        while (trace >> op >> std::hex >> addr) {
            cacheHierarchy.processMemoryAccess(op, addr);
        }

        cacheHierarchy.outputResults();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

bool parseParameters(int argc, char **argv) {
  // Read Parameters
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'p':
        isPreFetching = 1;
        break;
      case 'f':
        isFIFO = 1;
        break;
      case 'v':
        isVictimCache = 1;
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

void printUsage() { printf("Usage: CacheSim trace-file\n"); }