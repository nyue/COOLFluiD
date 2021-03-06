#ifndef COOLFluiD_Physics_LTE_Euler2DLTEDemixPuvtToCons_hh
#define COOLFluiD_Physics_LTE_Euler2DLTEDemixPuvtToCons_hh

//////////////////////////////////////////////////////////////////////////////

#include "Framework/VarSetTransformer.hh"
#include "Framework/MultiScalarTerm.hh"
#include "NavierStokes/EulerTerm.hh"

//////////////////////////////////////////////////////////////////////////////

namespace COOLFluiD {

  namespace Physics {

    namespace LTE {

//////////////////////////////////////////////////////////////////////////////

/**
 * This class represents a transformer of variables from primitive
 * [p u v T] to conservative variables with LTE and Demixing
 *
 * @author Andrea Lani
 * @author Janos Molnar
 *
 */
class Euler2DLTEDemixPuvtToCons : public Framework::VarSetTransformer {
public:
  
  typedef Framework::MultiScalarTerm<NavierStokes::EulerTerm> EulerLTEDemixTerm;
  
  /**
   * Default constructor without arguments
   */
  Euler2DLTEDemixPuvtToCons(Common::SafePtr<Framework::PhysicalModelImpl> model);

  /**
   * Default destructor
   */
  ~Euler2DLTEDemixPuvtToCons();
  
  /**
   * Transform a state into another one
   */
  void transform(const Framework::State& state, Framework::State& result);
  
  /**
   * Transform a state into another one from reference precomputed
   * values (physical data)associated to the given state
   */
  void transformFromRef(const RealVector& data, Framework::State& result);
    
private:

  /// acquaintance of the PhysicalModel
  Common::SafePtr<EulerLTEDemixTerm> _model;
  
  /// array to store density, enthalpy and energy
  RealVector _dhe;
  
  /// Vector storing the elemental composition
  RealVector _ye;

}; // end of class Euler2DLTEDemixPuvtToCons

//////////////////////////////////////////////////////////////////////////////

    } // namespace LTE

  } // namespace Physics

} // namespace COOLFluiD

//////////////////////////////////////////////////////////////////////////////

#endif // COOLFluiD_Physics_LTE_Euler2DLTEDemixPuvtToCons_hh
