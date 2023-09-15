//-----------------------------------------------------------------------------
// Entaro ChucK Developer!
// This is a Chugin boilerplate, generated by chuginate!
//-----------------------------------------------------------------------------

// this should align with the correct versions of these ChucK files
#include "chuck_dl.h"
#include "chuck_def.h"
#include "Helmholtz_dsp.h"

// general includes
#include <stdio.h>
#include <limits.h>

#define DEF_FIDELITY 0.95
#define DEF_SENSITIVITY 0.003
#define DEF_OVERLAP 2
#define DEF_BIAS 0.2
#define DEF_FRAME 2048

// declaration of chugin constructor
CK_DLL_CTOR(pitchtrack_ctor);
// declaration of chugin desctructor
CK_DLL_DTOR(pitchtrack_dtor);

// example of getter/setter
CK_DLL_MFUN(pitchtrack_getFreq);
CK_DLL_MFUN(pitchtrack_setFidelity);
CK_DLL_MFUN(pitchtrack_getFidelity);
CK_DLL_MFUN(pitchtrack_setSensitivity);
CK_DLL_MFUN(pitchtrack_getSensitivity);
CK_DLL_MFUN(pitchtrack_setOverlap);
CK_DLL_MFUN(pitchtrack_getOverlap);
CK_DLL_MFUN(pitchtrack_setFrame);
CK_DLL_MFUN(pitchtrack_getFrame);
CK_DLL_MFUN(pitchtrack_setBias);
CK_DLL_MFUN(pitchtrack_getBias);

// for Chugins extending UGen, this is mono synthesis function for 1 sample
CK_DLL_TICK(pitchtrack_tick);

// this is a special offset reserved for Chugin internal data
t_CKINT pitchtrack_data_offset = 0;

// class definition for internal Chugin data
// (note: this isn't strictly necessary, but serves as example
// of one recommended approach)

class PitchTrack
{
public:
  PitchTrack( t_CKFLOAT fs)
  {
    _SR = fs;
    _freq = 0;
    _fidelity = DEF_FIDELITY;
    _sensitivity = DEF_SENSITIVITY;
    _overlap = DEF_OVERLAP;
    _bias = DEF_BIAS;
    _frame = DEF_FRAME;
    
    _buffer = new float[_frame];
    _null_buffer = new float[_frame];
    _index = 0;
    
    for (int i = 0; i < _frame; i++)
      {
	_buffer[i] = 0.0;
	_null_buffer[i] = 0.0;
      }
    
    _helmholtz = new Helmholtz();
    _helmholtz->setbias(DEF_BIAS);
    _helmholtz->setoverlap(DEF_OVERLAP);
    _helmholtz->setminRMS(DEF_SENSITIVITY);
  }

  ~PitchTrack ()
  {
    free(_buffer);
    free(_null_buffer);
    delete _helmholtz;
  }

  // for Chugins extending UGen
  SAMPLE tick( SAMPLE in )
  {
    _buffer[_index] = in;
    _index = (_index + 1) % _frame;
    if (_index == 0)
      {
	_helmholtz->iosamples(_buffer, _null_buffer, _frame);
	
	float testfreq = _SR / (float)_helmholtz->getperiod();
	float testfidelity = (float) _helmholtz->getfidelity();
	if (testfidelity >= _fidelity)
	  {
	    _freq = testfreq;
	  }
      }
    return in;
  }
  
  float getFreq()
  {
	return _freq;
  }

  float setFidelity (t_CKFLOAT f)
  {
    _fidelity = f;
    return f;
  }

  float getFidelity () { return _fidelity; }
  
  float setSensitivity (t_CKFLOAT f)
  {
	_sensitivity = f;
	_helmholtz->setminRMS(_sensitivity);
	return f;
  }

  float getSensitivity () { return _sensitivity; }
  
  int setOverlap (t_CKINT i)
  {
	_overlap = i;
	_helmholtz->setoverlap(i);
	return i;
  }

  int getFrame() { return _frame; }

  int setFrame (t_CKINT i)
  {
    int pow2 = 128;
    while (pow2 < i) pow2 *= 2;
    _frame = pow2;
    delete _buffer;
    delete _null_buffer;
    _buffer = new float[_frame];
    _null_buffer = new float[_frame];
    _index = 0;
    
    for (int i = 0; i < _frame; i++)
      {
	_buffer[i] = 0.0;
	_null_buffer[i] = 0.0;
      }    
    return _frame;
  }

  int getOverlap() { return _overlap; }

  float setBias (t_CKFLOAT f)
  {
	_bias = f;
	_helmholtz->setbias(f);
	return f;
  }

  float getBias () { return _bias; }

private:
  // instance data
  t_CKFLOAT _freq;
  t_CKFLOAT _fidelity;
  t_CKFLOAT _sensitivity;
  t_CKFLOAT _overlap;
  t_CKFLOAT _bias;
  float *_buffer;
  float *_null_buffer;
  t_CKINT _index;
  t_CKINT _frame;
  Helmholtz *_helmholtz;
  t_CKFLOAT _SR;
};


// query function: chuck calls this when loading the Chugin
// NOTE: developer will need to modify this function to
// add additional functions to this Chugin
CK_DLL_QUERY( PitchTrack )
{
    // hmm, don't change this...
    QUERY->setname(QUERY, "PitchTrack");
    
    // begin the class definition
    // can change the second argument to extend a different ChucK class
    QUERY->begin_class(QUERY, "PitchTrack", "UGen");

    // register the constructor (probably no need to change)
    QUERY->add_ctor(QUERY, pitchtrack_ctor);
    // register the destructor (probably no need to change)
    QUERY->add_dtor(QUERY, pitchtrack_dtor);

    QUERY->doc_class(QUERY, "PitchTrack is a monophonic autocorrelation pitch tracker with a fast response and extremely high accuracy, even at low frequencies. It is adapted from [helmholtz~] for Pd by Katja, documented at http://www.katjaas.nl/helmholtz/helmholtz.html");

    // add examples
    QUERY->add_ex(QUERY, "effects/autotune.ck");
    QUERY->add_ex(QUERY, "analysis/PitchTrack.ck");

    // for UGen's only: add tick function
    QUERY->add_ugen_func(QUERY, pitchtrack_tick, NULL, 1, 1);
    
    // NOTE: if this is to be a UGen with more than 1 channel, 
    // e.g., a multichannel UGen -- will need to use add_ugen_funcf()
    // and declare a tickf function using CK_DLL_TICKF

    // example of adding setter method
    QUERY->add_mfun(QUERY, pitchtrack_setFidelity, "float", "fidelity");
    // example of adding argument to the above method
    QUERY->add_arg(QUERY, "float", "arg");

    QUERY->doc_func(QUERY, "Set the threshold for certainty about the result. A highly periodic signal (i.e. one that has a strong pitch center) should produce a result with a high fidelity, which a non-periodic signal (eg noise) will have a very low fidelity. Setting this parameter close to 1 should reduce the number of inaccurate reports. [0-1], default 0.95.");

    // example of adding setter method
    QUERY->add_mfun(QUERY, pitchtrack_setSensitivity, "float", "sensitivity");
    // example of adding argument to the above method
    QUERY->add_arg(QUERY, "float", "arg");
    QUERY->doc_func(QUERY, "Set the minimum RMS value to trigger a pitch calculation. Setting this parameter low forces PitchTrack to attempt to find the pitch of even very quiet sounds. Higher values will cause it to trigger only on louder notes. [0-1], default 0.003.");

    // example of adding setter method
    QUERY->add_mfun(QUERY, pitchtrack_setOverlap, "int", "overlap");
    // example of adding argument to the above method
    QUERY->add_arg(QUERY, "int", "arg");
    QUERY->doc_func(QUERY, "Set how much to overlap successive analysis frames. Higher values should produce smoother values, at the cost of an increase of CPU load. [1-?], default 2.");

    // example of adding setter method
    QUERY->add_mfun(QUERY, pitchtrack_setFrame, "int", "frame");
    // example of adding argument to the above method
    QUERY->add_arg(QUERY, "int", "arg");
    QUERY->doc_func(QUERY, "Set size of FFT frame for analysis. Smaller values result in lower latency and high responsiveness but less accuracy. Higher values result in considerably greater CPU load. Values that aren't powers of 2 get rounded up to the next power of 2. Recommend 512, 1024, or 2048. [128-?], default 2048.");

    // example of adding setter method
    QUERY->add_mfun(QUERY, pitchtrack_setBias, "float", "bias");
    // example of adding argument to the above method
    QUERY->add_arg(QUERY, "float", "arg");

    QUERY->doc_func(QUERY, "Set the bias. Katja's pitch tracker introduces a small bias to help with the tracking. See the link above.");

    // example of adding getter method
    QUERY->add_mfun(QUERY, pitchtrack_getFreq, "float", "get");
    QUERY->doc_func(QUERY, "Get calculated frequency.");

    QUERY->add_mfun(QUERY, pitchtrack_getFidelity, "float", "fidelity");
    QUERY->doc_func(QUERY, "Get the threshold for certainty about the result. A highly periodic signal (i.e. one that has a strong pitch center) should produce a result with a high fidelity, which a non-periodic signal (eg noise) will have a very low fidelity. Setting this parameter close to 1 should reduce the number of inaccurate reports. [0-1], default 0.95.");

    QUERY->add_mfun(QUERY, pitchtrack_getSensitivity, "float", "sensitivity");
    QUERY->doc_func(QUERY, "Get the minimum RMS value to trigger a pitch calculation. Setting this parameter low forces PitchTrack to attempt to find the pitch of even very quiet sounds. Higher values will cause it to trigger only on louder notes. [0-1], default 0.003.");


    QUERY->add_mfun(QUERY, pitchtrack_getOverlap, "int", "overlap");
    QUERY->doc_func(QUERY, "Get how much to overlap successive analysis frames. Higher values should produce smoother values, at the cost of an increase of CPU load. [1-?], default 2.");

    QUERY->add_mfun(QUERY, pitchtrack_getFrame, "int", "frame");
    QUERY->doc_func(QUERY, "Get size of FFT frame for analysis. Smaller values result in lower latency and high responsiveness but less accuracy. Higher values result in considerably greater CPU load. Values that aren't powers of 2 get rounded up to the next power of 2. Recommend 512, 1024, or 2048. [128-?], default 2048.");

    QUERY->add_mfun(QUERY, pitchtrack_getBias, "float", "bias");
    QUERY->doc_func(QUERY, "Get the bias. Katja's pitch tracker introduces a small bias to help with the tracking. See the link above.");
    
    // this reserves a variable in the ChucK internal class to store
    // referene to the c++ class we defined above
    pitchtrack_data_offset = QUERY->add_mvar(QUERY, "int", "@pt_data", false);

    // end the class definition
    // IMPORTANT: this MUST be called!
    QUERY->end_class(QUERY);

    // wasn't that a breeze?
    return TRUE;
}


// implementation for the constructor
CK_DLL_CTOR(pitchtrack_ctor)
{
    // get the offset where we'll store our internal c++ class pointer
    OBJ_MEMBER_INT(SELF, pitchtrack_data_offset) = 0;
    
    // instantiate our internal c++ class representation
    PitchTrack * bcdata = new PitchTrack(API->vm->get_srate(API, SHRED));
    
    // store the pointer in the ChucK object member
    OBJ_MEMBER_INT(SELF, pitchtrack_data_offset) = (t_CKINT) bcdata;
}


// implementation for the destructor
CK_DLL_DTOR(pitchtrack_dtor)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // check it
    if( bcdata )
    {
        // clean up
        delete bcdata;
        OBJ_MEMBER_INT(SELF, pitchtrack_data_offset) = 0;
        bcdata = NULL;
    }
}


// implementation for tick function
CK_DLL_TICK(pitchtrack_tick)
{
    // get our c++ class pointer
    PitchTrack * c = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
 
    // invoke our tick function; store in the magical out variable
    if(c) *out = c->tick(in);

    // yes
    return TRUE;
}

// example implementation for setter
CK_DLL_MFUN(pitchtrack_setFidelity)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // set the return value
    RETURN->v_float = bcdata->setFidelity(GET_NEXT_FLOAT(ARGS));
}

// example implementation for setter
CK_DLL_MFUN(pitchtrack_setSensitivity)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // set the return value
    RETURN->v_float = bcdata->setSensitivity(GET_NEXT_FLOAT(ARGS));
}

// example implementation for setter
CK_DLL_MFUN(pitchtrack_setOverlap)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // set the return value
    RETURN->v_float = bcdata->setOverlap(GET_NEXT_INT(ARGS));
}

CK_DLL_MFUN(pitchtrack_setFrame)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // set the return value
    RETURN->v_float = bcdata->setFrame(GET_NEXT_INT(ARGS));
}

// example implementation for setter
CK_DLL_MFUN(pitchtrack_setBias)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // set the return value
    RETURN->v_float = bcdata->setBias(GET_NEXT_FLOAT(ARGS));
}

// example implementation for getter
CK_DLL_MFUN(pitchtrack_getFreq)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // set the return value
    RETURN->v_float = bcdata->getFreq();
}

// example implementation for getter
CK_DLL_MFUN(pitchtrack_getSensitivity)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // set the return value
    RETURN->v_float = bcdata->getSensitivity();
}

// example implementation for getter
CK_DLL_MFUN(pitchtrack_getFidelity)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // set the return value
    RETURN->v_float = bcdata->getFidelity();
}

// example implementation for getter
CK_DLL_MFUN(pitchtrack_getBias)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // set the return value
    RETURN->v_float = bcdata->getBias();
}

// example implementation for getter
CK_DLL_MFUN(pitchtrack_getOverlap)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // set the return value
    RETURN->v_int = bcdata->getOverlap();
}

// example implementation for getter
CK_DLL_MFUN(pitchtrack_getFrame)
{
    // get our c++ class pointer
    PitchTrack * bcdata = (PitchTrack *) OBJ_MEMBER_INT(SELF, pitchtrack_data_offset);
    // set the return value
    RETURN->v_int = bcdata->getFrame();
}

