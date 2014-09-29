#include "FiniteVolume/FiniteVolume.hh"
#include "FiniteVolumeMHD/FiniteVolumeMHD.hh"
#include "Common/SafePtr.hh"
#include "MHD/MHD3DProjectionVarSet.hh"
#include "UnsteadySuperInletFromInputMHD3DProjection.hh"
#include "Framework/MethodCommandProvider.hh"
#include "Framework/SubSystemStatus.hh"

//////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace COOLFluiD::Framework;
using namespace COOLFluiD::Common;
using namespace COOLFluiD::Physics::MHD;

//////////////////////////////////////////////////////////////////////////////

namespace COOLFluiD {

  namespace Numerics {

    namespace FiniteVolume {

    	namespace FiniteVolumeMHD {

//////////////////////////////////////////////////////////////////////////////

MethodCommandProvider<UnsteadySuperInletFromInputMHD3DProjection, CellCenterFVMData, FiniteVolumeMHDModule> unsteadySuperInletFromInputMHD3DProjectionFVMCCProvider("UnsteadySuperInletFromInputMHD3DProjectionFVMCC");

//////////////////////////////////////////////////////////////////////////////

void UnsteadySuperInletFromInputMHD3DProjection::defineConfigOptions(Config::OptionList& options)
{
   options.addConfigOption< std::string >("ACEDataFileName","Name of Input File from where to read the inlet solar wind data from ACE satellite.");
   options.addConfigOption< CFreal >("beginTimeAtRestart","Begin time of the overall unsteady simulation that uses ACE data input file in case of restart.");
   options.addConfigOption< CFreal >("maxTime","Maximum time of the unsteady simulation.");
   options.addConfigOption< CFint, Config::DynamicOption<> >
     ("readInputFileFlag","Interacive flag telling whether to read the file.");
   options.addConfigOption< bool > ("useTimeInterpolation","Flag telling whether to use time interpolation.");
}

//////////////////////////////////////////////////////////////////////////////

UnsteadySuperInletFromInputMHD3DProjection::UnsteadySuperInletFromInputMHD3DProjection(const std::string& name) :
  SuperInlet(name),
  _varSet(CFNULL),
  _t(),
  _rho(),
  _u(),
  _v(),
  _w(),
  _bx(),
  _by(),
  _bz(),
  _dataInnerState(),
  _p()
{
  addConfigOptionsTo(this);

  _nameInputFile = "ACE";
   setParameter("ACEDataFileName",&_nameInputFile);

  _beginTime = 0.0;
   setParameter("beginTimeAtRestart",&_beginTime);

  _maxTime = 0.0;
  setParameter("maxTime",&_maxTime);

  _flagReadInputFile = 1;
  setParameter("readInputFileFlag",&_flagReadInputFile);

  _useTimeInterpolation = true;
  setParameter("useTimeInterpolation",&_useTimeInterpolation);
}

//////////////////////////////////////////////////////////////////////////////

UnsteadySuperInletFromInputMHD3DProjection::~UnsteadySuperInletFromInputMHD3DProjection()
{
}

//////////////////////////////////////////////////////////////////////////////

boost::filesystem::path UnsteadySuperInletFromInputMHD3DProjection::constructFilename()
{
  boost::filesystem::path fpath(_nameInputFile);

  CFout << "Reading ACE solar wind data from: " << fpath.string() << "\n";

  return fpath;
}

//////////////////////////////////////////////////////////////////////////////

void UnsteadySuperInletFromInputMHD3DProjection::readInputFileNoInterpolation() 
{ 
  SelfRegistPtr<Environment::FileHandlerInput> fhandle = 
    Environment::SingleBehaviorFactory<Environment::FileHandlerInput>::getInstance().create();
  ifstream& inputFile = fhandle->open(constructFilename());
  
  _t.resize(1); 
  _rho.resize(1); 
  _u.resize(1); 
  _v.resize(1); 
  _w.resize(1); 
  _bx.resize(1); 
  _by.resize(1); 
  _bz.resize(1); 
  _p.resize(1); 
  
  inputFile >> _rho[0] >> _u[0] >> _v[0] >> _w[0] >> _bx[0] >> _by[0] >> _bz[0] >> _p[0];
  inputFile.close();
}

////////////////////////////////////////////////////////////////////////////// 

void UnsteadySuperInletFromInputMHD3DProjection::readInputFile()
{
  // The input file is opened and closed twice: first for determining the number of lines in the input file; second for reading the data

  SelfRegistPtr<Environment::FileHandlerInput> fhandle = Environment::SingleBehaviorFactory<Environment::FileHandlerInput>::getInstance().create();
  ifstream& inputFile = fhandle->open(constructFilename());

  std::string line, t, rho, u, v, w, bx, by, bz, p;
  CFuint nbLines = 0;
  RealVector tFile, rhoFile, uFile, vFile, wFile, bxFile, byFile, bzFile, pFile;

  if (inputFile.is_open())
  {
    while (true)
    {
      if (inputFile.eof()) 
      {
      	break;
      }
      getline (inputFile,line);
      ++nbLines;
    }
  }

  fhandle->close();

  ifstream& inpFile = fhandle->open(constructFilename());

  tFile.resize(nbLines-1);
  rhoFile.resize(nbLines-1);
  uFile.resize(nbLines-1);
  vFile.resize(nbLines-1);
  wFile.resize(nbLines-1);
  bxFile.resize(nbLines-1);
  byFile.resize(nbLines-1);
  bzFile.resize(nbLines-1);
  pFile.resize(nbLines-1);

  for (CFuint iLine = 0; iLine < nbLines-1; ++iLine) {
      getline(inpFile,line);
      istringstream liness( line );
      getline(liness, t, ' ');
      getline(liness, rho, ' ');
      getline(liness, u, ' ');
      getline(liness, v, ' ');
      getline(liness, w, ' ');
      getline(liness, bx, ' ');
      getline(liness, by, ' ');
      getline(liness, bz, ' ');
      getline(liness, p, ' ');

      tFile[iLine] = atof(t.c_str());
      rhoFile[iLine] = atof(rho.c_str());
      uFile[iLine] = atof(u.c_str());
      vFile[iLine] = atof(v.c_str());
      wFile[iLine] = atof(w.c_str());
      bxFile[iLine] = atof(bx.c_str());
      byFile[iLine] = atof(by.c_str());
      bzFile[iLine] = atof(bz.c_str());
      pFile[iLine] = atof(p.c_str());
  }

  fhandle->close();

  const CFreal maxDT = SubSystemStatusStack::getActive()->getDT();

  const CFreal tInitial = tFile[0];

  for (CFuint iLine = 0; iLine < tFile.size(); ++iLine) {
        tFile[iLine] -= tInitial;
  }
  
  CFuint nbTimeSteps = int((_maxTime-_beginTime)/maxDT);
  CFreal minDifference, time, t0, t1, rho0, rho1, u0, u1, v0, v1, w0, w1, bx0, bx1, by0, by1, bz0, bz1, p0, p1;
  CFuint indexMinDifference = 0;

  _t.resize(nbTimeSteps+1);
  _rho.resize(nbTimeSteps+1);
  _u.resize(nbTimeSteps+1);
  _v.resize(nbTimeSteps+1);
  _w.resize(nbTimeSteps+1);
  _bx.resize(nbTimeSteps+1);
  _by.resize(nbTimeSteps+1);
  _bz.resize(nbTimeSteps+1);
  _p.resize(nbTimeSteps+1);

  // 2- WATCH OUT FOR INDEXING ESPECIALLY WHEN BEGINTIME IS NOT ZERO SO CHECK BELOW
  for (CFuint iLine = 0; iLine < (nbTimeSteps+1); ++iLine) {
        minDifference = 1.0e6;
        time = _beginTime+(iLine*maxDT);
        for (CFuint jLine = 0; jLine < tFile.size(); ++jLine) {
                if (minDifference > fabs(time-tFile[jLine]))
                {
                        minDifference = fabs(time-tFile[jLine]);
                        indexMinDifference = jLine;
                }
        }
        if ((time-tFile[indexMinDifference]) >= 0.0)
        {
                t0 = tFile[indexMinDifference];
                t1 = tFile[indexMinDifference+1];
                rho0 = rhoFile[indexMinDifference];
                rho1 = rhoFile[indexMinDifference+1];
                u0 = uFile[indexMinDifference];
                u1 = uFile[indexMinDifference+1];
                v0 = vFile[indexMinDifference];
                v1 = vFile[indexMinDifference+1];
                w0 = wFile[indexMinDifference];
                w1 = wFile[indexMinDifference+1];
                bx0 = bxFile[indexMinDifference];
                bx1 = bxFile[indexMinDifference+1];
                by0 = byFile[indexMinDifference];
                by1 = byFile[indexMinDifference+1];
                bz0 = bzFile[indexMinDifference];
                bz1 = bzFile[indexMinDifference+1];
                p0 = pFile[indexMinDifference];
                p1 = pFile[indexMinDifference+1];
        }
        if ((time-tFile[indexMinDifference]) < 0.0)
        {
                t0 = tFile[indexMinDifference-1];
                t1 = tFile[indexMinDifference];
                rho0 = rhoFile[indexMinDifference-1];
                rho1 = rhoFile[indexMinDifference];
                u0 = uFile[indexMinDifference-1];
                u1 = uFile[indexMinDifference];
                v0 = vFile[indexMinDifference-1];
                v1 = vFile[indexMinDifference];
                w0 = wFile[indexMinDifference-1];
                w1 = wFile[indexMinDifference];
                bx0 = bxFile[indexMinDifference-1];
                bx1 = bxFile[indexMinDifference];
                by0 = byFile[indexMinDifference-1];
                by1 = byFile[indexMinDifference];
                bz0 = bzFile[indexMinDifference-1];
                bz1 = bzFile[indexMinDifference];
                p0 = pFile[indexMinDifference-1];
                p1 = pFile[indexMinDifference];
        }
        _rho[iLine] = rho0 + ((rho1-rho0)/(t1-t0))*(time-t0);
        _u[iLine] = u0 + ((u1-u0)/(t1-t0))*(time-t0);
        _v[iLine] = v0 + ((v1-v0)/(t1-t0))*(time-t0);
        _w[iLine] = w0 + ((w1-w0)/(t1-t0))*(time-t0);
        _bx[iLine] = bx0 + ((bx1-bx0)/(t1-t0))*(time-t0);
        _by[iLine] = by0 + ((by1-by0)/(t1-t0))*(time-t0);
        _bz[iLine] = bz0 + ((bz1-bz0)/(t1-t0))*(time-t0);
        _p[iLine] = p0 + ((p1-p0)/(t1-t0))*(time-t0);
  }
}

//////////////////////////////////////////////////////////////////////////////

void UnsteadySuperInletFromInputMHD3DProjection::setup()
{
  SuperInlet::setup();
 
  _varSet = getMethodData().getUpdateVar().d_castTo<MHD3DProjectionVarSet>(); 
  _varSet->getModel()->resizePhysicalData(_dataInnerState);

  cf_assert(_varSet.isNotNull());
}

//////////////////////////////////////////////////////////////////////////////

void UnsteadySuperInletFromInputMHD3DProjection::unsetup()
{
  SuperInlet::unsetup();
}

//////////////////////////////////////////////////////////////////////////////

void UnsteadySuperInletFromInputMHD3DProjection::configure ( Config::ConfigArgs& args )
{
  SuperInlet::configure(args);

}

//////////////////////////////////////////////////////////////////////////////


void UnsteadySuperInletFromInputMHD3DProjection::preProcess()
{
  if (_flagReadInputFile == 1) { 
    // if this is a parallel simulation, only ONE process at a time reads the file !!!
    if (PE::GetPE().IsParallel()) { 
      PE::GetPE().setBarrier(); 
      for (CFuint i = 0; i < PE::GetPE().GetProcessorCount(); ++i) { 
	if (i == PE::GetPE().GetRank ()) { 
	  (_useTimeInterpolation) ? readInputFile() : readInputFileNoInterpolation(); 
	} 
	
	PE::GetPE().setBarrier(); 
      } 
    } 
    else { 
      (_useTimeInterpolation) ? readInputFile() : readInputFileNoInterpolation();
    }
    
    _flagReadInputFile = 0;
  } 
}
	  
////////////////////////////////////////////////////////////////////////////// 


void UnsteadySuperInletFromInputMHD3DProjection::setGhostState(GeometricEntity *const face)
{
  State *const innerState = face->getState(0);
  State *const ghostState = face->getState(1);

  //Set the ghost state value in terms of the non-dimensional input file data

  const CFreal gamma = _varSet->getModel()->getGamma();
  const CFreal gammaMinus1 = gamma - 1.0;
  const CFreal currentTime = SubSystemStatusStack::getActive()->getCurrentTime();
  const CFreal maxDT = SubSystemStatusStack::getActive()->getDT();
  
  const CFuint currentTimeIndex = (_useTimeInterpolation) ? int(currentTime/maxDT) : 0;
  
  //if ((_beginTime+currentTime) > _maxTime)
  //{
  //	Environment::CFEnv::getInstance().terminate();
  //}

  // set the physical data starting from the inner state
  _varSet->computePhysicalData(*innerState, _dataInnerState);

  (*ghostState)[0] = 2.*_rho[currentTimeIndex] - _dataInnerState[MHDTerm::RHO];
  (*ghostState)[1] = 2.*_u[currentTimeIndex] - _dataInnerState[MHDTerm::VX];
  (*ghostState)[2] = 2.*_v[currentTimeIndex] - _dataInnerState[MHDTerm::VY];
  (*ghostState)[3] = 2.*_w[currentTimeIndex] - _dataInnerState[MHDTerm::VZ];
  (*ghostState)[4] = 2.*_bx[currentTimeIndex] - _dataInnerState[MHDTerm::BX];
  (*ghostState)[5] = 2.*_by[currentTimeIndex] - _dataInnerState[MHDTerm::BY];
  (*ghostState)[6] = 2.*_bz[currentTimeIndex] - _dataInnerState[MHDTerm::BZ];
  (*ghostState)[7] = 2.*_p[currentTimeIndex] - _dataInnerState[MHDTerm::P];
  (*ghostState)[8] = -_dataInnerState[MHDProjectionTerm::PHI];
}

//////////////////////////////////////////////////////////////////////////////

    } // namespace FiniteVolumeMHD
    
   } // namespace FiniteVolume

  } // namespace Numerics

} // namespace COOLFluiD