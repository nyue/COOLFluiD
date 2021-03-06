#include "MultiFluidMHD/MultiFluidMHD.hh"
#include "MultiFluidMHD/DiffMFMHD2DRhoiViTi.hh"
#include "Common/StringOps.hh"
#include "Environment/ObjectProvider.hh"
#include "MultiFluidMHD/EulerMFMHDTerm.hh"

//////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace COOLFluiD::Framework;
using namespace COOLFluiD::Common;

//////////////////////////////////////////////////////////////////////////////

namespace COOLFluiD {

  namespace Physics {

    namespace MultiFluidMHD {

//////////////////////////////////////////////////////////////////////////////

Environment::ObjectProvider<DiffMFMHD2DRhoiViTi, DiffusiveVarSet, MultiFluidMHDModule, 2>
nsMHD2DRhoiViTiProvider("MultiFluidMHD2DRhoiViTi");

//////////////////////////////////////////////////////////////////////////////

DiffMFMHD2DRhoiViTi::DiffMFMHD2DRhoiViTi(const std::string& name, Common::SafePtr<Framework::PhysicalModelImpl> model) :
  DiffMFMHD2DVarSet(name, model),
  m_eulerModel(model->getConvectiveTerm().d_castTo<PTERM>()),
  m_density()
{
  const CFuint endEM = 8;
  const CFuint nbSpecies = m_eulerModel->getNbScalarVars(0);
  const CFuint nbMomentum   = m_eulerModel->getNbScalarVars(1);
  const CFuint nbEnergyEqs  = m_eulerModel->getNbScalarVars(2);
  const CFuint totalNbEqs = nbSpecies + nbMomentum + nbEnergyEqs + 8;   
  
  vector<std::string> names(totalNbEqs);
  
  names[0] = "Bx";
  names[1] = "By";
  names[2] = "Bz";
  names[3] = "Ex";
  names[4] = "Ey";
  names[5] = "Ez";
  names[6] = "Psi";
  names[7] = "Phi";
  
  
  for (CFuint ie = 0; ie < nbSpecies; ++ie) {
    names[endEM + ie] = "rho" + StringOps::to_str(ie);
  }

  for (CFuint ie = 0; ie < nbSpecies; ++ie) {
    names[endEM + nbSpecies + 2*ie]     = "U" + StringOps::to_str(ie);
    names[endEM + nbSpecies + 2*ie + 1] = "V" + StringOps::to_str(ie);
  } 
  
  for (CFuint ie = 0; ie < nbSpecies; ++ie) {
    names[endEM + nbSpecies + nbMomentum + ie] = "T" + StringOps::to_str(ie);
  } 
  
  setVarNames(names);
}

//////////////////////////////////////////////////////////////////////////////

DiffMFMHD2DRhoiViTi::~DiffMFMHD2DRhoiViTi()
{
}

//////////////////////////////////////////////////////////////////////////////

void DiffMFMHD2DRhoiViTi::setGradientVars(const vector<RealVector*>& states,
					  RealMatrix& values,
					  const CFuint stateSize)

{
  using namespace std;
  using namespace COOLFluiD::Framework;
  
  const CFuint nbValues = values.nbRows();
  for (CFuint i = 0; i < nbValues; ++i) {
    for (CFuint j = 0; j < stateSize; ++j) {
      values(i,j) = (*states[j])[i];
    }
  }
}

//////////////////////////////////////////////////////////////////////////////

void DiffMFMHD2DRhoiViTi::setGradientVarGradients(const vector<RealVector*>& states,
                                                  const vector< vector<RealVector*> >& stateGradients,
                                                  vector< vector<RealVector*> >& gradVarGradients,
                                                  const CFuint stateSize)
{
  using namespace std;
  using namespace COOLFluiD::Framework;
  
  cf_assert(stateGradients.size() > 0);
  const CFuint nbValues = stateGradients[0].size();
  for (CFuint i = 0; i < nbValues; ++i)
  {
    for (CFuint j = 0; j < stateSize; ++j)
    {
      *gradVarGradients[j][i] = *stateGradients[j][i];
    }
  }
}

//////////////////////////////////////////////////////////////////////////////

void DiffMFMHD2DRhoiViTi::setStateGradients(const vector<RealVector*>& states,
                                            const vector< vector<RealVector*> >& gradVarGradients,
                                            vector< vector<RealVector*> >& stateGradients,
                                            const CFuint stateSize)
{
  using namespace std;
  using namespace COOLFluiD::Framework;
  
  cf_assert(stateGradients.size() > 0);
  const CFuint nbValues = stateGradients[0].size();
  for (CFuint i = 0; i < nbValues; ++i)
    {
      for (CFuint j = 0; j < stateSize; ++j)
	{
	  *stateGradients[j][i] = *gradVarGradients[j][i];
	}
  }
}

//////////////////////////////////////////////////////////////////////////////

RealVector& DiffMFMHD2DRhoiViTi::getDynViscosityVec(const RealVector& state,
						    const vector<RealVector*>& gradients)
{
  return getModel().getDynViscosityDim();
}

//////////////////////////////////////////////////////////////////////////////

CFreal DiffMFMHD2DRhoiViTi::getDynViscosity(const RealVector& state,
                                            const vector<RealVector*>& gradients)
{
  return getModel().getDynViscosityDim().max();
}

//////////////////////////////////////////////////////////////////////////////

CFreal DiffMFMHD2DRhoiViTi::getThermConductivity(const RealVector& state,
                                      const CFreal& dynViscosity)
{
  return getModel().getThermConductivityDim().max();
}

//////////////////////////////////////////////////////////////////////////////

CFreal DiffMFMHD2DRhoiViTi::getDensity(const RealVector& state)
{
  const CFuint nbSpecies = m_eulerModel->getNbScalarVars(0);
  const CFuint endEM = 8;
  CFreal rho = 0.0;
  for (CFuint ie = 0; ie < nbSpecies; ++ie) {
    rho += state[endEM + ie];
  }
  return rho;
}

//////////////////////////////////////////////////////////////////////////////

void DiffMFMHD2DRhoiViTi::setGradientState(const RealVector& state)
{
  cf_assert(_gradState.size() == state.size());
  _gradState = state;
}

//////////////////////////////////////////////////////////////////////////////

    } // namespace MultiFluidMHD

  } // namespace Physics

} // namespace COOLFluiD

//////////////////////////////////////////////////////////////////////////////
