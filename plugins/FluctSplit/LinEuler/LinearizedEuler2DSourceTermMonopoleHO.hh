#ifndef COOLFluiD_Numerics_FluctSplit_LinearizedEuler2DSourceTermMonopoleHO_hh
#define COOLFluiD_Numerics_FluctSplit_LinearizedEuler2DSourceTermMonopoleHO_hh

//////////////////////////////////////////////////////////////////////////////

#include "FluctSplit/ComputeSourceTermFSM.hh"
#include "LinEuler/LinEulerTerm.hh"
#include "LinEuler/LinEuler2DVarSet.hh"

//////////////////////////////////////////////////////////////////////////////

namespace COOLFluiD {

  namespace Framework {
    class State; 
  }

  namespace FluctSplit {

    class InwardNormalsData;

//////////////////////////////////////////////////////////////////////////////

/**
 * This class represents a source term for LinearizedEuler2D MONOPOLE
 * 
 * @author  Lilla Edit Koloszar
 */
class LinearizedEuler2DSourceTermMonopoleHO : public ComputeSourceTermFSM {
public:
  
  /**
   * Constructor
   * @see LinEuler2D
   */
  LinearizedEuler2DSourceTermMonopoleHO(const std::string& name);
  
  /**
   * Default destructor
   */
  ~LinearizedEuler2DSourceTermMonopoleHO();
  
  /**
   * Set up private data and data of the aggregated classes 
   * in this command before processing phase
   */
  void setup();

  /**
   * unSet up private data and data of the aggregated classes 
   * in this command before processing phase
   */
//   void unsetup();
  
  /**
   * Set the variable set
   * @pre the input pointer is non const to allow dynamic_cast
   */
  void setVarSet(Common::SafePtr<Framework::ConvectiveVarSet> varSet);
  
  /**
   * Compute the source term
   */ 
  virtual void computeSourceFSM(Framework::GeometricEntity *const cell,
				RealVector& source,
				const FluctSplit::InwardNormalsData& normalsData);

/**
  * Define configuration options in the CFcase config file
  */

  static void defineConfigOptions(Config::OptionList& options);
  
private: // data
  
  /// corresponding variable set
  Common::SafePtr<Physics::LinearizedEuler::LinEuler2DVarSet> _varSet;

  /// acquaintance of the PhysicalModel
  Common::SafePtr<Physics::LinearizedEuler::LinEulerTerm> _model;
  
  /// vector to store temporary result
  RealVector _temp;

  /// width of the source
  CFreal m_alpha;

  /// amplitude of the source
  CFreal m_eps;

  /// frequency of the source
  CFreal m_freq;

  /// location vector of the source
  std::vector<CFreal> _sourceloc;

 /// quadrature points per sub-element
  RealVector qd0;
  RealVector qd1;
  RealVector qd2;
  RealVector wqd;


  /// states at quadrature points
  std::vector<Framework::State*> qdstates;

  /// value of the sources at each points;
  std::vector<RealVector> source_value;
 }; // end of class LinEuler2DSourceTerm

//////////////////////////////////////////////////////////////////////////////

    } // namespace FluctSplit

} // namespace COOLFluiD

//////////////////////////////////////////////////////////////////////////////

#endif // COOLFluiD_Numerics_FluctSplit_LinearizedEuler2DSourceTermMonopoleHO_hh
