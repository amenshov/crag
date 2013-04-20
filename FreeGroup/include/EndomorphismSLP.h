/*
 * EndomorphsimSLP.h
 *
 *  Created on: Jan 29, 2013
 *      Author: pmorar
 */

#ifndef CRAG_FREE_GROUPS_ENDOMORPHISM_SLP_H_
#define CRAG_FREE_GROUPS_ENDOMORPHISM_SLP_H_

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <functional>
#include <assert.h>
#include "slp.h"

namespace crag {

/**
 * Represents a free group endomorphism using straight-line programs.
 * @tparam TerminalSymbol terminal symbols class
 */
template<typename TerminalSymbol = int>
class EndomorphismSLP {
public:

  typedef typename std::map<TerminalSymbol, slp::Vertex>::size_type size_type;
  typedef typename std::map<TerminalSymbol, slp::Vertex>::iterator iterator;
  typedef typename std::map<TerminalSymbol, slp::Vertex>::const_iterator const_iterator;
  typedef typename std::map<TerminalSymbol, slp::Vertex>::value_type symbol_image_pair_type;

  //use default copy/move constructors/assignments

  //! Returns the identity automorphism
  static EndomorphismSLP identity() {
    return EndomorphismSLP();
  }

  //! Returns the automorphism inverting the specified terminal symbbol.
  /**
   * @param symbol inverted terminal symbol, must be > 0
   */
  static EndomorphismSLP inverter(const TerminalSymbol& symbol) {
    assert(is_positive_terminal_symbol(symbol));
    return EndomorphismSLP(symbol);
  }

  //! Returns the automorphism mapping #symbol to the product #symbol #right_multiplier.
  /**
   * @param symbol mapped terminal symbol, must be > 0
   * @param	right_multiplier	right multiplier
   */
  static EndomorphismSLP right_multiplier(const TerminalSymbol& symbol, const TerminalSymbol& right_multiplier) {
    assert(is_positive_terminal_symbol(symbol));
    assert(symbol != right_multiplier);
    EndomorphismSLP tmp;
    tmp.images_.insert(std::make_pair(symbol,
      slp::NonterminalVertex(TerminalVertex(symbol), TerminalVertex(right_multiplier))));
    return tmp;
  }

  //! Returns the automorphism mapping #symbol to the product #left_multiplier #symbol.
  /**
   * @param  left_multiplier  left multiplier
   * @param symbol mapped terminal symbol, must be > 0
   */
  static EndomorphismSLP left_multiplier(const TerminalSymbol& left_multiplier, const TerminalSymbol& symbol) {
    assert(is_positive_terminal_symbol(symbol));
    assert(left_multiplier != symbol);
    EndomorphismSLP tmp;
    tmp.images_.insert(std::make_pair(symbol,
      slp::NonterminalVertex(TerminalVertex(left_multiplier), TerminalVertex(symbol))));
    return tmp;
  }

  //! Applies provided function to each inverter, left and right multiplier for the given rank
  template<typename Func>
  static void for_each_basic_morphism(int rank, Func f) {
    assert(rank > 0);
    for (int i = 1; i <= rank; ++i)
      f(EndomorphismSLP<TerminalSymbol>::inverter(i));
    for (int i = 1; i <= rank; ++i)
      for (int j = -static_cast<int>(rank); j <= rank; ++j)
        if (j != i && j != -i && j != 0) {
          f(EndomorphismSLP<TerminalSymbol>::left_multiplier(j, i));
          f(EndomorphismSLP<TerminalSymbol>::right_multiplier(i, j));
        }
  }

  //! Returns the composition of endomorphisms specified by the range
  template<typename Iterator>
  static EndomorphismSLP composition(Iterator begin, Iterator end) {
    return identity().compose_with(begin, end);
  }

  //! Returns the composition of #num endomorphisms produced by #generator
  /**
   * @param num      the number of endomorphisms to compose
   * @param generator endomorphisms generator (using operator())
   */
  template<typename Generator>
  static EndomorphismSLP composition(unsigned int num, Generator& generator) {
    return identity().compose_with(num, generator);
  }

  //! Compose with the given endomorphism.
  EndomorphismSLP& operator*=(const EndomorphismSLP& a);

  //! Compose with endomorphisms specified by the range.
  template<typename Iterator>
  EndomorphismSLP& compose_with(Iterator begin, Iterator end) {
    for(;begin != end; ++begin)
      (*this) *= *begin;
    return *this;
  }

  //! Compose with #num automorphism produced by #generator
  /**
   * @param num the number of endomorphisms to compose with
   * @param generator endomorphisms generator (using operator())
   */
  template<typename Generator>
  EndomorphismSLP& compose_with(unsigned int num, Generator& generator) {
    for (unsigned int i = 0; i < num; ++i)
      (*this) *= generator();
    return *this;
  }

  //! Conjugate with the endomorphisms from the given range.
  /**
   * @param num the number of endomorphisms to compose with
   * @param generator endomorphisms generator (using operator())
   * @return a this a^{-1}
   */
  template<typename Iterator>
  const EndomorphismSLP conjugate_with(Iterator begin, Iterator end) const {
    std::vector<EndomorphismSLP> inverses;
    EndomorphismSLP e(*this);
    for(;begin != end; ++begin) {
      auto endomorphism = *begin;
      e *= endomorphism;
      inverses.push_back(endomorphism.inverse());
    }
    return EndomorphismSLP::composition(inverses.rbegin(), inverses.rend()) * e;
  }

  //! Conjugate with the automorphisms generated by the given generator.
  /**
   * @param num the number of endomorphisms to compose with
   * @param generator endomorphisms generator (using operator())
   * @return a this a^{-1}
   */
  template<typename Generator>
  const EndomorphismSLP conjugate_with(unsigned int num, Generator& generator) const {
    std::vector<EndomorphismSLP> inverses;
    inverses.reserve(num);
    EndomorphismSLP e(*this);
    for (unsigned int i = 0; i < num; ++i) {
      auto endomorphism = generator();
      e *= endomorphism;
      inverses.push_back(endomorphism.inverse());
    }
    return EndomorphismSLP::composition(inverses.rbegin(), inverses.rend()) * e;
  }

  //! Returns the automorphisms inverse
  /**
   * Currently supporsts only inverter and left and right multipliers.
   * @return automorphisms inverse
   * @throws std::invalid_argument if can not invert the automorphism
   */
  EndomorphismSLP inverse() const;

  //! Returns the automorphisms with freely reduced images.
  EndomorphismSLP free_reduction() const {
    EndomorphismSLP result;
    slp::MatchingTable mt;
    std::unordered_map<slp::Vertex, slp::Vertex> reduced_vertices;
    for_each_non_trivial_image([&] (const symbol_image_pair_type& pair) {
      result.images_.insert(std::make_pair(pair.first, slp::reduce(pair.second, &mt, &reduced_vertices)));
    });
    return result;
  }


  //! Returns the image of the terminal.
  slp::VertexWord<TerminalSymbol> image_word(const TerminalSymbol& t) const {
    bool is_positive = is_positive_terminal_symbol(t);
    return slp::VertexWord<TerminalSymbol>(is_positive ? image(t) : image(-t).negate());
  }

  //! Returns the root of the straight-line program representing the positive terminal symbol.
  /**
   * @param t positive terminal symbol
   */
  slp::Vertex image(const TerminalSymbol& t) const {
    assert(is_positive_terminal_symbol(t));
    auto result = images_.find(t);
    if (result == images_.end()) //if it is not in images_, then it is the identity map.
      return TerminalVertex(t);
    else
      return result->second;
  }

  //! Returns the maximal terminal symbol with non-identity image.
  TerminalSymbol max_non_trivial_image_symbol() const {
    if (images_.empty())
      return TerminalSymbol();
    return images_.rbegin()->first;
  }

  size_type non_trivial_images_num() const {
    unsigned int n = 0;
    for (auto key_image: images_) {
      auto key = key_image.first;
      slp::Vertex image = key_image.second;
      if (image.height() == 1 && TerminalVertex(image).terminal_symbol() == key)
        continue;
      ++n;
    }
    return n;
  }

  //! Returns range (begin, end) of non-trivial images containg pairs (symbol, image)
  std::pair<iterator, iterator> non_trivial_images_range() {
    return std::make_pair(images_.begin(), images_.end());
  }

  //! Returns range (begin, end) of non-trivial images containg pairs (symbol, image)
  std::pair<const_iterator, const_iterator> non_trivial_images_range() const {
    return std::make_pair(images_.begin(), images_.end());
  }

  iterator begin() {
    return images_.begin();
  }

  iterator end() {
    return images_.end();
  }

  const_iterator begin() const {
    return images_.begin();
  }

  const_iterator end() const {
    return images_.end();
  }

  //! Applies the given function to each pair of non-trivial images (symbol, image)
  template<class Function>
  Function for_each_non_trivial_image(Function fn) {
    return std::move(std::for_each(images_.begin(), images_.end(), fn));
  }

  //! Applies the given function to each pair of non-trivial images (symbol, image)
  template<class Function>
  Function for_each_non_trivial_image(Function fn) const {
    return std::move(std::for_each(images_.begin(), images_.end(), fn));
  }

  //! Returns true, if the automorphism is identity, and returns false otherwise.
  bool is_identity() const {
    bool is_id = true;
    for_each_non_trivial_image([&is_id](const symbol_image_pair_type& pair) {
            if (is_id)
              is_id = TerminalVertex(pair.first) == slp::reduce(pair.second);
          });
    return is_id;
  }

  bool operator==(const EndomorphismSLP& a) const;

  bool operator!=(const EndomorphismSLP& a) const {
    return !(this->operator ==(a));
  }

  void save_to(std::ostream* out) const;
  static EndomorphismSLP load_from(std::istream* in);

private:
  typedef typename slp::TerminalVertexTemplate<TerminalSymbol> TerminalVertex;

  //! Checks whether the symbol is not inverse.
  static bool is_positive_terminal_symbol(const TerminalSymbol& symbol) {
    static const TerminalSymbol null_symbol = TerminalSymbol();
    return symbol > null_symbol;
  }

  //! Helper method that copies #vertex such that the children of the new vertex are images of the children of #vertex
  /**
   * It assumes that if #vertex is non-terminal, then #images contains its children images
   */
  slp::Vertex map_vertex(const slp::Vertex& vertex, const std::unordered_map<slp::Vertex, slp::Vertex>& images) const;

  //! The default constructor.
  EndomorphismSLP() {}

  EndomorphismSLP(const TerminalSymbol& inverted) {
    images_.insert(std::make_pair(inverted,
      TerminalVertex(inverted).negate()));
  }

  //! The representation of images of terminal symbols by straight-line programs
  /**
   * If there is no entry for a given terminal symbol, then its image is the terminal itself.
   * Also we use {@code std::map} instead of {@code std::unordered_map} because we would like
   * to iterate over the keys in the specific order defined by operator < for TerminalSymbol.
   */
  std::map<TerminalSymbol, slp::Vertex> images_;
};

//! Compose given endomorphisms.
/**
 * Returns const to behave consistently with built-in types.
 */
template<typename TerminalSymbol>
const EndomorphismSLP<TerminalSymbol> operator*(const EndomorphismSLP<TerminalSymbol>& e1, const EndomorphismSLP<TerminalSymbol>& e2) {
  return EndomorphismSLP<TerminalSymbol>(e1) *= e2;
}

//! Find the maximal height of SLPs, representing the endomorphism
template<typename TerminalSymbol>
unsigned int height(const EndomorphismSLP<TerminalSymbol>& e) {
  unsigned int h = 0;
  auto pick_max_height = [&h] (const typename EndomorphismSLP<TerminalSymbol>::symbol_image_pair_type& v) {
    const unsigned int v_h = v.second.height();
    if (v_h > h)
      h = v_h;
  };
  e.for_each_non_trivial_image(pick_max_height);
  return h;
}

//! Find the total number of vertices in SLPs, representing the endomorphism
template<typename TerminalSymbol>
unsigned int slp_vertices_num(const EndomorphismSLP<TerminalSymbol>& e) {
  std::unordered_set<slp::Vertex> visited_vertices;

  auto acceptor = [&visited_vertices] (const slp::inspector::InspectorTask& task) {
    return visited_vertices.find(task.vertex) == visited_vertices.end();
  };

  auto inspect_root =[&acceptor,&visited_vertices] (const typename EndomorphismSLP<TerminalSymbol>::symbol_image_pair_type& v) {
    slp::Inspector<slp::inspector::Postorder, decltype(acceptor)> inspector(v.second, acceptor);
    while (!inspector.stopped()) {
      visited_vertices.insert(inspector.vertex());
      inspector.next();
    }
  };

  e.for_each_non_trivial_image(inspect_root);
  return visited_vertices.size();
}



//! Automorphisms generator
/**
 * @tparam TerminalSymbol          terminal symbol representation. We suppose it has a constructor taking index
 * @tparam RandomEngine            engine generating uniformly random non-negative numbers. See std::random library documentation.
 * @tparam TerminalSymbolIndexType terminal symbol index type
 */
template <typename TerminalSymbol = int,
  typename RandomEngine = std::default_random_engine>
class UniformAutomorphismSLPGenerator {
public:
  typedef int index_type;

  //! Constructs a generator of automorphisms of the free group of the given rank.
  /**
   * @param rank free group rank > 0
   */
  UniformAutomorphismSLPGenerator(index_type rank)
    : UniformAutomorphismSLPGenerator(rank, std::make_shared<RandomEngine>(), nullptr)
  {}

  //! Constructs a generator of automorphisms of the free group of the given rank.
  /**
   * @param rank free group rank > 0
   * @param seed random engine seed for creation of a new one
   */
  explicit UniformAutomorphismSLPGenerator(index_type rank, typename RandomEngine::result_type seed)
    : UniformAutomorphismSLPGenerator(rank, ::std::make_shared<RandomEngine>(seed), nullptr)
  {}

  //! Constructs a generator of automorphisms of the free group of the given rank.
  /**
   * @param rank free group rank > 0
   * @param random_engine random engine
   */
  explicit UniformAutomorphismSLPGenerator(index_type rank, RandomEngine* random_engine)
    : UniformAutomorphismSLPGenerator(rank, nullptr, random_engine)
  {}

  //! Set the probability to generate an inverter of terminal symbol. By default it corresponds to uniform distribution.
  void set_inverters_probability(double inverters_probability) {
    assert(inverters_probability >= 0 && inverters_probability <= 1);
    inverters_probability_ = inverters_probability;
  }

  //! Generates a random automorphism.
  EndomorphismSLP<TerminalSymbol> operator()() {
    double p = real_distr_(*random_engine_);
    if (p <= inverters_probability_) {//generate an inverter
      index_type val = inverter_distr_(*random_engine_);
      return EndomorphismSLP<TerminalSymbol>::inverter(TerminalSymbol(1 + val));
    } else {//generate a multiplier
      index_type val = multiplier_distr_(*random_engine_);
      const bool right_multiplier = (val % 2) == 0;
      val >>= 1;
      const int mapped_symbol_index = 1 + ( val % RANK );
      const TerminalSymbol mapped_symbol(mapped_symbol_index);
      const int multiplier_index = 1 + ( val / RANK );
      const TerminalSymbol multiplier(multiplier_index < mapped_symbol_index ? multiplier_index : multiplier_index + 1);

      if (right_multiplier)
        return EndomorphismSLP<TerminalSymbol>::right_multiplier(mapped_symbol, multiplier);
      else
        return EndomorphismSLP<TerminalSymbol>::left_multiplier(multiplier, mapped_symbol);
    }
  }


private:
  const index_type RANK;
  const index_type RIGHT_MULTIPLIERS_COUNT;
  const index_type LEFT_MULTIPLIERS_COUNT = RIGHT_MULTIPLIERS_COUNT;
  const index_type MULTIPLIERS_COUNT = RIGHT_MULTIPLIERS_COUNT + LEFT_MULTIPLIERS_COUNT;
  const index_type INVERTERS_COUNT;
  const index_type COUNT = MULTIPLIERS_COUNT + INVERTERS_COUNT;

  const double DEFAULT_INVERTER_PROBABILITY = static_cast<double>(INVERTERS_COUNT) / COUNT;

  UniformAutomorphismSLPGenerator(index_type rank, const ::std::shared_ptr<RandomEngine>& random_engine_ptr, RandomEngine* random_engine)
      : RANK(rank),
        RIGHT_MULTIPLIERS_COUNT(rank * (rank - 1)),
        INVERTERS_COUNT(rank),
        random_engine_ptr_(random_engine_ptr),
        random_engine_(random_engine_ptr ? random_engine_ptr.get() : random_engine),
        inverter_distr_(0, INVERTERS_COUNT - 1),
        multiplier_distr_(0, MULTIPLIERS_COUNT - 1),
        real_distr_(),//interval [0, 1)
        inverters_probability_(DEFAULT_INVERTER_PROBABILITY) {
      assert(rank > 0);
    }

  ::std::shared_ptr<RandomEngine> random_engine_ptr_;
  RandomEngine* random_engine_;
  std::uniform_int_distribution<index_type> inverter_distr_;
  std::uniform_int_distribution<index_type> multiplier_distr_;
  std::uniform_real_distribution<double> real_distr_;
  double inverters_probability_;
};


//template<typename TerminalSymbol, typename Func>
//static void EndomorphismSLP<TerminalSymbol>::for_each_basic_morphism(unsigned int rank, Func f) {
//  for (int i = 1; i <= rank; ++i)
//    f(EndomorphismSLP<TerminalSymbol>::inverter(i));
//  for (int i = 1; i <= rank; ++i)
//    for (int j = 1; j <= rank; ++j)
//      if (j != i && j != -i && j != 0) {
//        f(EndomorphismSLP<TerminalSymbol>::left_multiplier(j, i));
//        f(EndomorphismSLP<TerminalSymbol>::right_multiplier(i, j));
//      }
//}


template <typename TerminalSymbol>
EndomorphismSLP<TerminalSymbol> EndomorphismSLP<TerminalSymbol>::inverse() const {
  if (images_.size() == 0) //identity
    return *this;
  if (images_.size() > 1)
    throw std::invalid_argument("Unsupported endomorphism with more than one non-trivial terminal image!");

  auto img = images_.begin();
  auto symbol = img->first;
  slp::Vertex image = img->second;
  if (image.height() > 2)
    throw std::invalid_argument("Unsupported endomorphism with unsupported slp height > 2!");

  if (image.height() == 1) {//inverter
    return *this;
  }

  TerminalVertex left(image.left_child());
  TerminalVertex right(image.right_child());

  auto left_symbol = left.terminal_symbol();
  auto right_symbol = right.terminal_symbol();

  if (! (left_symbol == symbol || right_symbol == symbol))
    throw std::invalid_argument("Unsupported endomorphism not mapping the symbol to the product of another one and itself!");

  if (left_symbol == symbol) {
    return right_multiplier(symbol, -right_symbol);
  } else {
    return left_multiplier(-left_symbol, symbol);
  }
}

template <typename TerminalSymbol>
bool EndomorphismSLP<TerminalSymbol>::operator==(const EndomorphismSLP<TerminalSymbol>& a) const {
  if (this == &a)
    return true;
  if (non_trivial_images_num() != a.non_trivial_images_num())
    return false;
  auto images = non_trivial_images_range();
  auto a_images = a.non_trivial_images_range();

  //checking that the same letters have non-trivial images
  auto img_iterator = images_.begin();
  auto a_img_iterator = a.images_.begin();
  while (img_iterator != images_.end()) {
    if (img_iterator->first != a_img_iterator->first)
      return false;
    ++img_iterator;
    ++a_img_iterator;
  }

  //checking that the images are the same
  slp::MatchingTable mt;
  img_iterator = images_.begin();
  a_img_iterator = a.images_.begin();
  while (img_iterator != images_.end()) {
    slp::VertexWord<TerminalSymbol> word(img_iterator->second);
    slp::VertexWord<TerminalSymbol> a_word(a_img_iterator->second);
    if (!word.is_equal_to(a_word, &mt))
      return false;
    ++img_iterator;
    ++a_img_iterator;
  }

  return true;
}

template <typename TerminalSymbol>
EndomorphismSLP<TerminalSymbol>& EndomorphismSLP<TerminalSymbol>::operator*=(const EndomorphismSLP<TerminalSymbol>& a) {
  std::unordered_map<slp::Vertex, slp::Vertex> new_vertices;//a's vertices to new vertices correspondence

  for (auto root_entry: a.images_) {//mapping vertices of #a to new ones
    slp::map_vertices(root_entry.second, &new_vertices,
                      std::bind(&EndomorphismSLP<TerminalSymbol>::map_vertex, *this, std::placeholders::_1, std::placeholders::_2));
  }

  //replacing roots
  std::map<TerminalSymbol, slp::Vertex> new_images;
  for (auto root_entry: a.images_) {
    auto new_root = new_vertices.find(root_entry.second)->second;
      new_images.insert(std::make_pair(root_entry.first, new_root));
  }
  //adding images that were not inserted
  for (auto root_entry: images_) {
    if (new_images.find(root_entry.first) == new_images.end())//it was not mapped by a
      new_images.insert(root_entry);
  }

  swap(images_, new_images);
  return *this;
}

template<typename TerminalSymbol>
slp::Vertex EndomorphismSLP<TerminalSymbol>::map_vertex(const slp::Vertex& vertex, const std::unordered_map<slp::Vertex, slp::Vertex>& images) const {
  if (!vertex)
    return vertex;//Mapping null vertex

  if (vertex.height() == 1) {//the vertex is terminal so map it to our corresponding root
    const TerminalSymbol& symbol = TerminalVertex(vertex).terminal_symbol();
    bool is_positive = is_positive_terminal_symbol(symbol);
    auto positive_symbol = is_positive ? symbol : -symbol;
    slp::Vertex v = image(positive_symbol);
    if (TerminalVertex(positive_symbol) == v)//if id map
      return vertex;
    return is_positive ? v : v.negate();
  } else {//for a nonterminal we already processed its children because postorder inspector
    auto left_val = images.find(vertex.left_child());
    auto right_val = images.find(vertex.right_child());
    assert(left_val != images.end() && right_val != images.end());
    auto left = left_val->second;
    auto right = right_val->second;
    if (left == vertex.left_child() && right == vertex.right_child()) //if children were not copied, then we should not copy vertex
      return vertex;
    return slp::NonterminalVertex(left, right);
  }
}

template<typename TerminalSymbol>
void EndomorphismSLP<TerminalSymbol>::save_to(std::ostream* out) const {
  size_t vertex_num = 0;

  std::unordered_map<size_t, std::pair<size_t, size_t>> non_terminals;
  std::unordered_map<size_t, TerminalSymbol> terminals;

  //we save the order in wich vertices occur during postorder inspection because we want to save them in this
  //order so it would be easier to reconstruct the automorphism later
  std::vector<size_t> non_terminals_order;
  std::vector<size_t> terminals_order;

  auto processor = [&] (const slp::Vertex& vertex, const std::unordered_map<slp::Vertex, size_t>& mapped_images) {
    if (vertex.height() == 1) {//the vertex is terminal
      const TerminalSymbol& symbol = TerminalVertex(vertex).terminal_symbol();
      terminals.insert(std::make_pair(vertex_num, symbol));
      terminals_order.push_back(vertex_num);
    } else {//nonterminal
      size_t left_val = mapped_images.find(vertex.left_child())->second;
      size_t right_val = mapped_images.find(vertex.right_child())->second;
      non_terminals.insert(std::make_pair(vertex_num, std::make_pair(left_val, right_val)));
      non_terminals_order.push_back(vertex_num);
    }
    return vertex_num++;
  };

  std::unordered_map<slp::Vertex, size_t> vertex_numbers;
  for (auto root_entry: images_) {
    slp::map_vertices(root_entry.second, &vertex_numbers,
                      processor);
  }

  //writing
  //"number of nontrivial images" "number of terminals" "number of non-terminals"
  *out << images_.size() << " " << terminals.size() << " " << non_terminals.size() << std::endl;

  //writing terminal symbols
  //"terminal vertex index" "terminal symbol or inverse"
  for (size_t terminal_vertex_index: terminals_order) {
    auto terminal = *(terminals.find(terminal_vertex_index));
    *out << terminal.first << " " << terminal.second << std::endl;
  }

  //writing non-terminals
  //"vertex index" "left child index" "right child index"
  for (size_t non_terminal_vertex_index: non_terminals_order) {
    auto non_terminal = *(non_terminals.find(non_terminal_vertex_index));
    *out << non_terminal.first << " " << non_terminal.second.first << " " << non_terminal.second.second << std::endl;
  }

  //writing roots
  //"terminal symbol" "vertex index"
  for (auto root_entry: images_)
    *out << root_entry.first << " " << vertex_numbers.find(root_entry.second)->second << std::endl;
}

template<typename TerminalSymbol>
EndomorphismSLP<TerminalSymbol> EndomorphismSLP<TerminalSymbol>::load_from(std::istream* in) {
  size_t roots_num;
  size_t terminals_num;
  size_t non_terminals_num;
  *in >> roots_num >> terminals_num >> non_terminals_num;

  std::unordered_map<size_t, slp::Vertex> vertices;
  for (int i = 0; i < terminals_num; ++i) {
    size_t index;
    TerminalSymbol image;
    *in >> index >> image;
    in->ignore();
    vertices.insert(std::make_pair(index, TerminalVertex(image)));
  }

  for (int i = 0; i < non_terminals_num; ++i) {
    size_t index;
    size_t l_index;
    size_t r_index;
    *in >> index >> l_index >> r_index;
    in->ignore();
    auto left= vertices.find(l_index)->second;
    auto right = vertices.find(r_index)->second;
    vertices.insert(std::make_pair(index, slp::NonterminalVertex(left, right)));
  }

  EndomorphismSLP e;
  for (int i = 0; i < roots_num; ++i) {
    TerminalSymbol key;
    size_t index;
    *in >> key >> index;
    in->ignore();
    auto root = vertices.find(index)->second;
    e.images_.insert(std::make_pair(key, root));
  }
  return e;
}


} /* namespace crag */
#endif /* CRAG_FREE_GROUPS_ENDOMORPHISM_SLP_H_ */
