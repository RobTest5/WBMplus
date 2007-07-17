/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDRelHumidity.c

balazs.fekete@unh.edu

*******************************************************************************/

#include<stdio.h>
#include<cm.h>
#include<MF.h>
#include<MD.h>

static int _MDInAtMeanID       = MFUnset;
static int _MDInVaporPressID   = MFUnset;

static int _MDOutRelHumidityID = MFUnset;

static void _MDRelHumidity (int itemID) {
// Input
	float airT;      // air temperature [degree C]
	float vPress;    // vapor pressure [kPa]
// Output
	float relHumid;  // relative humidity in fraction (0.0 - 1.0)
// Local
	float sVPress;   // saturated vapor pressure [kPa]

	if (MFVarTestMissingVal (_MDInAtMeanID,     itemID) ||
		 MFVarTestMissingVal (_MDInVaporPressID, itemID)) { MFVarSetMissingVal (_MDOutRelHumidityID,itemID); return; }

	airT   = MFVarGetFloat (_MDInAtMeanID,     itemID);
	vPress = MFVarGetFloat (_MDInVaporPressID, itemID);

 	sVPress = MDPETlibVPressSat (airT);

 	relHumid = vPress < sVPress ? vPress / sVPress : 0.99;

	MFVarSetFloat (_MDOutRelHumidityID, itemID, relHumid);
}

enum { MDinput, MDcalc };

int MDRelHumidityDef () {
	int optID = MDinput;
	const char *optStr, *optName = MDVarRelHumidity;
	const char *options [] = { MDInputStr, MDCalculateStr, (char *) NULL };
	
	if (_MDOutRelHumidityID != MFUnset) return (_MDOutRelHumidityID);

	MFDefEntering ("Relative Humidity");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutRelHumidityID = MFVarGetID (MDVarRelHumidity,  "%", MFInput, MFState, MFBoundary); break;
		case MDcalc:
			if (((_MDInAtMeanID       = MFVarGetID (MDVarAirTemperature, "degC",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	    		 ((_MDInVaporPressID   = MFVarGetID (MDVarVaporPressure,  "kPa",   MFInput, MFState, MFBoundary)) == CMfailed) ||
		  		 ((_MDOutRelHumidityID = MFVarGetID (MDVarRelHumidity,    "mm",    MFOutput,MFState, MFBoundary)) == CMfailed))
				return (CMfailed);
			_MDOutRelHumidityID = MFVarSetFunction (_MDOutRelHumidityID,_MDRelHumidity);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Relative Humidity");
	return (_MDOutRelHumidityID);
}
