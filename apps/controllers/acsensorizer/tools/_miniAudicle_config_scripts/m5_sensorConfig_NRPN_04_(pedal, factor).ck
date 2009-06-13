// *****************************************************************
// ******************** m5 sensorizer NRPN control *****************
// *****************************************************************
// 
// created 2006 by Michael Markert, http://www.audiocommander.de
// control m5 sensorizer app by NRPN messages
// for MBHP & MIOS see http://www.midibox.org
// this script is for miniAudicle: http://audicle.cs.princeton.edu/mini/
//
//
// v0.0.1 08/2006	created
//


// controlTypes
0 => int ENA;
1 => int PEDAL;
2 => int AUTOSENS;
3 => int INVERT;
4 => int RELEASEDETECT;
16 => int SLW;
17 => int MIN;
18 => int MAX;
19 => int FACTOR;
33 => int FROM;
34 => int TO;
112 => int CH;
113 => int CC;

// dataValues
0 => int PED_OFF;
2 => int PED_PANIC;
4 => int PED_FILTER;

0 => int AUTOSENS_OFF;
1 => int AUTOSENS_MIN;
2 => int AUTOSENS_MAX;



// ============================== values here =========================

// ** select midi I/O **
// min port number
0 => int min_port_num;
// mout port number
1 => int mout_port_num;


// ** select sensor **
0 => int sensorNum;	// 0..7

// ** select controlType **
//PEDAL => int controlType;
FACTOR => int controlType;


// ** select dataValue **
//PED_FILTER => int dataValue;
//PED_OFF => int dataValue;
4 => int dataValue;



// **************** don't change anything from here on *************
// create midi obj
//MidiIn min;
// connect to port
//min.open(min_port_num);

// create midi obj
MidiOut mout;
// connect to port
mout.open(mout_port_num);

// send some data
MidiMsg msg;

if( !mout.open(mout_port_num) ) me.exit();

// send CC 99, NRPN MSB
176 => msg.data1;
99 => msg.data2;
sensorNum => msg.data3;	// select sensor# (0-7)
mout.send(msg);

// send CC 98, NRPN LSB
176 => msg.data1;
98 => msg.data2;
controlType => msg.data3;
mout.send(msg);

// send CC 6, DataEntry MSB
176 => msg.data1;
6 => msg.data2;
0 => msg.data3;
mout.send(msg);

// send CC 38, DataEntry LSB
176 => msg.data1;
38 => msg.data2;
dataValue => msg.data3;
mout.send(msg);
