
#ifndef FORMATSTRINGS
#define FORMATSTRINGS

#ifdef COLOURS
const std::string bold2 = "\033[1m";
const std::string green2 = "\033[32m";
const std::string underline2 = "\033[4m";
const std::string blue2 = "\033[34m";
const std::string red2 = "\033[31m";
const std::string white2 = "\033[37m";
const std::string noFormat2 = "\033[0m";
#else
const std::string bold2 = "";
const std::string green2 = "";
const std::string underline2 = "";
const std::string blue2 = "";
const std::string red2 = "";
const std::string white2 = "";
const std::string noFormat2 = "";

#endif



#ifdef DRAWBOX

const std::string topLeft="\xE2\x95\x94";
const std::string line="\xE2\x95\x90";
const std::string topRight="\xE2\x95\x97";
const std::string vertical="\xE2\x95\x91";
const std::string bottomLeft="\xE2\x95\x9A";
const std::string bottomRight="\xE2\x95\x9D";

const std::string leftDivider = "\xE2\x95\x9F";
const std::string rightDivider = "\xE2\x95\xA2";
const std::string thinLine = "\xE2\x94\x80";

#else
const std::string topLeft="";
const std::string line="";
const std::string topRight="";
const std::string vertical="";
const std::string bottomLeft="";
const std::string bottomRight="";

const std::string leftDivider = "";
const std::string rightDivider = "";
const std::string thinLine = "";

#endif



const std::string plusMinus="\xC2\xB1";

#endif
