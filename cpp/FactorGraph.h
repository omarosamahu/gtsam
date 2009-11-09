/**
 * @file    FactorGraph.h
 * @brief   Factor Graph Base Class
 * @author  Carlos Nieto
 * @author  Christian Potthast
 */

// \callgraph

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "Testable.h"
#include "BayesNet.h"

namespace gtsam {

	class Ordering;

	/**
	 * A factor graph is a bipartite graph with factor nodes connected to variable nodes.
	 * In this class, however, only factor nodes are kept around.
	 * 
	 * Templated on the type of factors and configuration.
	 */
	template<class Factor> class FactorGraph: public Testable<FactorGraph<Factor> > {
	public:
		typedef typename boost::shared_ptr<Factor> sharedFactor;
		typedef typename std::vector<sharedFactor>::iterator iterator;
		typedef typename std::vector<sharedFactor>::const_iterator const_iterator;

	protected:
		/** Collection of factors */
		std::vector<sharedFactor> factors_;

		/** For each variable a list of factor indices connected to it  */
		typedef std::map<std::string, std::list<int> > Indices;
		Indices indices_;

	public:

		/** Default constructor */
		FactorGraph() {}

		/** convert from Bayes net */
		template<class Conditional>
		FactorGraph(const BayesNet<Conditional>& bayesNet);

		/** print out graph */
		void print(const std::string& s = "FactorGraph") const;

		/** Check equality */
		bool equals(const FactorGraph& fg, double tol = 1e-9) const;

		/** STL like, return the iterator pointing to the first factor */
		inline const_iterator begin() const {
			return factors_.begin();
		}

		/** STL like, return the iterator pointing to the last factor */
		inline const_iterator end() const {
			return factors_.end();
		}

		/** clear the factor graph */
		inline void clear() {
			factors_.clear();
		}

		/** Get a specific factor by index */
		inline sharedFactor operator[](size_t i) const {
			return factors_[i];
		}

		/** return the number of factors and NULLS */
	  inline size_t size() const { return factors_.size();}

		/** return the number valid factors */
	  size_t nrFactors() const;

		/** Add a factor */
		void push_back(sharedFactor factor);

		/**
		 * Compute colamd ordering
		 */
		Ordering getOrdering() const;

    /**
     * Return indices for all factors that involve the given node
     * @param key the key for the given node
     */
    std::list<int> factors(const std::string& key) const;

    /**
     * find all the factors that involve the given node and remove them
     * from the factor graph
     * @param key the key for the given node
     */
		std::vector<sharedFactor> findAndRemoveFactors(const std::string& key);

    /**
     * extract and combine all the factors that involve a given node
     * @param key the key for the given node
     * @return the combined linear factor
     */
    sharedFactor removeAndCombineFactors(const std::string& key);

	private:

		/** Serialization function */
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version) {
			ar & BOOST_SERIALIZATION_NVP(factors_);
			ar & BOOST_SERIALIZATION_NVP(indices_);
		}
	}; // FactorGraph

	/** doubly templated functions */

	/**
   * Eliminate a single node yielding a Conditional
   * Eliminates the factors from the factor graph through findAndRemoveFactors
   * and adds a new factor on the separator to the factor graph
   */
	template<class Factor, class Conditional>
	boost::shared_ptr<Conditional> _eliminateOne(
			FactorGraph<Factor>& factorGraph,
			const std::string& key);

	/**
	 * eliminate factor graph using the given (not necessarily complete)
	 * ordering, yielding a chordal Bayes net and (partially eliminated) FG
	 */
	template<class Factor, class Conditional>
	BayesNet<Conditional>
	_eliminate(FactorGraph<Factor>& factorGraph, const Ordering& ordering);

  /**
   * static function that combines two factor graphs
   * @param const &fg1 Linear factor graph
   * @param const &fg2 Linear factor graph
   * @return a new combined factor graph
   */
	template<class Factor>
	FactorGraph<Factor> combine(const FactorGraph<Factor>& fg1, const FactorGraph<Factor>& fg2);

} // namespace gtsam

