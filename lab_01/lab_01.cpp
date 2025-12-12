#include <random>
#include <iostream>
#include <numeric>
#include <thread>
#include <chrono>
#include <iomanip>
#include <vector>

class Clicker
{
public:
  Clicker():
    start_(std::chrono::high_resolution_clock::now())
  {
  }

  double millisec() const
  {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::milli;

    auto t = high_resolution_clock::now();
    return duration< double, milli >(t - start_).count();
  }

private:
  std::chrono::time_point< std::chrono::high_resolution_clock > start_;
};

size_t getMonteCarloParam(size_t r, size_t amount, size_t seed)
{
  int start = 0 - r;
  int end = r;
  std::mt19937 gen(seed);
  std::uniform_real_distribution<> dis(start, end);
  size_t hit{ 0 };
  for (size_t i = 0; i < amount; i++)
  {
    std::pair< double, double > point{ dis(gen), dis(gen) };
    if ((point.first * point.first) + (point.second * point.second) <= r * r)
    {
      hit++;
    }
  }
  return hit;
}

void sumMonteCarloParam(size_t r, size_t i, size_t amount, size_t& hit, size_t seed)
{
  hit = getMonteCarloParam(r, amount, seed);
}

double getArea(size_t r, size_t hits, size_t tries)
{
  return (4 * r * r) * (double(hits) / tries);
}

int main(int argc, char** argv)
{
  if (argc < 2 || argc > 3)
  {
    std::cerr << "[INVALID PARAMETRS COUNT]\n";
    return 1;
  }

  int tries = std::atoi(argv[1]);
  if (tries <= 0)
  {
    std::cerr << "[NUMBER OF EXPEREMENTS SHOULD BE POSITIVE]\n";
    return 1;
  }
  if (argc == 3)
  {
    int seed = std::atoi(argv[2]);
    if (seed < 0)
    {
      std::cerr << "[NUMBER OF SRAND() SHOULD BE POSITIVE]\n";
      return 1;
    }
    std::srand(seed);
  }
  else
  {
    std::srand(0);
  }

  while (!(std::cin.eof()))
  {
    size_t r{ 0 }, tn{ 0 };
    if (!(std::cin >> r) || !(std::cin >> tn) || tn <= 0)
    {
      std::cerr << "[RADIUS AND THREEDS COUNT SHOULD BE POSITIVE]\n";
      return 1;
    }
    Clicker cl;
    std::vector< std::thread > ths;
    ths.reserve(tn - 1);
    size_t per_th = tries / tn;
    std::vector< size_t > params(tn, 0);
    double sum{ 0 };
    size_t i{ 0 };
    for (; i < tn - 1; ++i)
    {
      ths.emplace_back(sumMonteCarloParam, r, i * per_th, per_th, std::ref(params[i]), i);
    }
    sumMonteCarloParam(r, i * per_th, per_th + tries % tn, std::ref(params[i]), i);
    for (auto&& th : ths) th.join();
    sum = getArea(r, std::accumulate(params.begin(), params.end(), 0ull), tries);
    double total = cl.millisec();
    std::cout << std::fixed << std::setprecision(3) << total << ' ' << sum << '\n';
  }
}