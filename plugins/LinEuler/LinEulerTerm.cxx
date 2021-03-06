#include "LinEulerTerm.hh"

//////////////////////////////////////////////////////////////////////////////


using namespace std;
using namespace COOLFluiD::Framework;

//////////////////////////////////////////////////////////////////////////////

namespace COOLFluiD {

  namespace Physics {

    namespace LinearizedEuler {


//////////////////////////////////////////////////////////////////////////////

void LinEulerTerm::defineConfigOptions(Config::OptionList& options)
{
   options.addConfigOption< CFreal >("gamma","gamma of mean flow value.");
}

//////////////////////////////////////////////////////////////////////////////

///@Lilla changed for unhomogeneous flow

LinEulerTerm::LinEulerTerm(const std::string& name)
  : BaseTerm(name),
  _meanPathStates(CFNULL)
{
  addConfigOptionsTo(this);

  _gamma = 0.0;
  setParameter("gamma",&_gamma);
}

//////////////////////////////////////////////////////////////////////////////

LinEulerTerm::~LinEulerTerm()
{
}

//////////////////////////////////////////////////////////////////////////////

void LinEulerTerm::configure ( Config::ConfigArgs& args )
{
  BaseTerm::configure(args);
}

//////////////////////////////////////////////////////////////////////////////

void LinEulerTerm::resizePhysicalData(RealVector& physicalData)
{
  // resize the physical data
  cf_assert(getDataSize() > 0);
  physicalData.resize(getDataSize());
}

//////////////////////////////////////////////////////////////////////////////

void LinEulerTerm::setupPhysicalData()
{
  cf_assert(getDataSize() > 0);

  // set the size of each physical data in the StatesData
  resizePhysicalData(m_physicalData);
  resizePhysicalData(m_refPhysicalData);
}

//////////////////////////////////////////////////////////////////////////////

} // namespace LinearizedEuler

} // namespace Physics

} // namespace COOLFluiD

//////////////////////////////////////////////////////////////////////////////
