#ifndef COOLFluiD_Numerics_SpectralFV_NSQuadFreeFaceTermComputer_hh
#define COOLFluiD_Numerics_SpectralFV_NSQuadFreeFaceTermComputer_hh

//////////////////////////////////////////////////////////////////////////////

#include "NavierStokes/NavierStokesVarSet.hh"

#include "SpectralFV/QuadFreeFaceTermComputer.hh"

//////////////////////////////////////////////////////////////////////////////

namespace COOLFluiD {
  namespace SpectralFV {

//////////////////////////////////////////////////////////////////////////////

/**
 * This class represents a basic strategy that computes the face terms
 * in a quadrature-free way. This computer is specifically for Navier-Stokes.
 *
 * @author Kris Van den Abeele
 */
class NSQuadFreeFaceTermComputer : public QuadFreeFaceTermComputer {

public:  // methods

  /// Constructor
  NSQuadFreeFaceTermComputer(const std::string& name);

  /// Destructor
  ~NSQuadFreeFaceTermComputer();

  /// Gets the Class name
  static std::string getClassName()
  {
    return "NSQuadFreeFaceTermComputer";
  }

  /// Set up private data and data
  void setup();

  /// Unset up private data and data
  void unsetup();

  /**
   * compute the diffusive face term and the contribution to the update coefficient for this face
   * @pre reconstructFluxPntsStates(), reconstructFluxPntsGradients(), reconstructFaceAvgState(),
   *      setFaceTermData() and set the geometrical data of the face
   */
  void computeDiffFaceTermAndUpdateCoefContributions(RealVector& resUpdates,
                                                     CFreal& updateCoefContrL,
                                                     CFreal& updateCoefContrR);

protected: // data

  /// Navier-Stokes variable set
  Common::SafePtr< Physics::NavierStokes::NavierStokesVarSet > m_navierStokesVarSet;

}; // class NSQuadFreeFaceTermComputer

//////////////////////////////////////////////////////////////////////////////

  }  // namespace SpectralFV

}  // namespace COOLFluiD

//////////////////////////////////////////////////////////////////////////////

#endif  // COOLFluiD_Numerics_SpectralFV_NSQuadFreeFaceTermComputer_hh
