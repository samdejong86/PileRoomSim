#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

#ifndef XmlParser_h
#define XmlParser_h 1


class XmlParser{

public:
  static std::string startTag;
  static std::string endTag;
  static std::string xmlStartTag;
  static std::string xmlEndTag;


  XmlParser(){}

  XmlParser(std::string filename){
  //XmlParser(TString filename){
    
    //ifstream infile(filename); 
    //in.open(filename);

    std::ifstream in(filename);
    
    char str[600];
    
    std::string activeTag = "";
    std::string text;
    
    
    while(in) {
      in.getline(str, 256);    
      std::string st = str;    
      st = this->removeSpaces(st); 
 		
      if((st.compare(xmlStartTag))==0){
	std::cout<<"\nworking...\n";
      }
      else if((st.compare(xmlEndTag))==0){
	std::cout<<"done!\n";
      }else if ((st.compare(this->endTag))==0) {
	
      }
      else if (this->isXMLStartTag(st)) {
	activeTag = st;
      }
      else if (this->isXMLEndTag(st)) {
	text = this->removeSpaces(text);
	//G4cout<<activeTag<<"\t"<<text<<"\n";
	this->setXMLField(activeTag, text);
	text = "";
      }
      else
	text = text+ "\n" + st;
    }
    in.close();
  }

  std::string removeSpaces(std::string input)
  {
    int length = input.length();
    for (int i = length-1; i >= 0; --i) { //for (int i = 0; i < length; i++) {
      if(input[i] == ' '||input[i] == '\t'||input[i] == '\n')
	input.erase(i, 1);
    }
    return input;
  }
  
  double getValue(std::string tag){ 
    for(int i=0; i<(int)tags.size(); i++){
      if(tags[i].compare(tag)==0) return atof(vals[i].c_str());       
    }
    return 0;
  }
  
  std::string getStringValue(std::string tag){
    for(int i=0; i<(int)tags.size(); i++){
      if(tags[i].compare(tag)==0) return vals[i];       
    }
    return "";
}

  
  static bool isXMLStartTag(std::string aTagstring){
    std::string input = aTagstring;
    if(input.length()==0) return false;
    
    char last = input.at(input.length()-1);
    char first = input.at(0);
    
    if(first=='<'&&last=='>'&&input.find("</") == std::string::npos){
      return true;
    }
    return false;

  }

  static bool isXMLEndTag(std::string aTagstring){
    std::string input = aTagstring;
    if(input.length()==0) return false;
    
    char last = input.at(input.length()-1);
    char first = input.at(0);
    
    if(first=='<'&&last=='>'&&input.find("</") != std::string::npos){
      return true;
    }
    return false;
    
  }
  
  void setXMLField(std::string xmlField, std::string text) {
    if(xmlField.compare(startTag)==0||xmlField.compare(xmlStartTag)==0) return;
    if(xmlField.find("</") != std::string::npos) return;
    
    std::string input = xmlField;
    std::string datastring = text;
    input.erase(0,1);
    input.erase(input.length()-1,1);
    
    bool found=false;
    for(int i=0; i<(int)tags.size(); i++){
      if(tags[i].compare(input)==0){
	found=true;
	vals[i] = text;
      }
    }

    if(!found){
      tags.push_back(input);
      vals.push_back(text);
    }
    
  }
  
  void print(){
    for(int i=0; i<(int)tags.size(); i++) std::cout<<tags[i]<<"\t"<<vals[i]<<std::endl;
  }

 
 
 
 private:
  std::vector<std::string> tags;
  std::vector<std::string> vals;

 



};





/*
std::string XmlParser::startTag="<Active>";
std::string XmlParser::endTag="</Active>";

std::string XmlParser::xmlStartTag = "<xml>";
std::string XmlParser::xmlEndTag = "</xml>";
*/












#endif
