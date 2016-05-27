/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   superpos.h:
**      This file contains class and constant definitions which are used
**      in managing superpositions.  A superposition is a 1-d spectrum which
**      is overlaid on an existing display pane which is already displaying
**      an existing 1-d spectrum.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
#ifndef _SUPERPOS_H_INSTALLED
#define _SUPERPOS_H_INSTALLED

#include <string>

/*
** Superposition  class definition: A superposition is a spectrum number
** and a rendition selector and appropriate methods for accessing them.
*/

#define MAX_SUPERPOSITIONS     8

static const int Super_Undefined = -1;
class Superposition {
 public:
  /* Constructors:   */

  Superposition() { 
    spectrum = Super_Undefined;
  }
  Superposition(int spec, int r) {
    spectrum = spec;
    rendition= r;
  }
  Superposition(const std::string& name, int r) {
      spectrum = -1;
      rendition = r;
      specname = name;
  }

  Superposition(Superposition &s) {
    spectrum = s.spectrum;
    rendition= s.rendition;
    specname = s.specname;
  }
  /* Accessors: */

  int Representation()       { return rendition; }
  void Representation(int r) { rendition = r; }

  int Spectrum()          { return spectrum; }
  void Spectrum(int spec) { spectrum = spec; }

  void SpectrumName(const std::string& name) { specname = name; }
  std::string SpectrumName() const { return specname; }

 private:
  int spectrum;			/* Number of the spectrum. */
  int rendition;		/* Rendition selector. */
  std::string specname; /* name of spectrum */
};

/*
** SuperpositionList class definition:  A SuperpositionList is a list of
** superpositions along with sufficient methods and additional fields to
** add spectra to the list, clear the list and assign renditions.
*/
class SuperpositionList {
 public:
  /* Constructor(s): */

  SuperpositionList() {
    num    = 0;			/* Currently no spectrar. */
    lastrep= -1;		/* First representation is 0. */
  }
  SuperpositionList(SuperpositionList &l);

  /* Accessors etc. */

  int Count() { return num; }	/* Return number of spectra in list. */
  void Add(int spec);		/* Add a new superposition.          */
  void Add(const std::string& name); /* add a new superposition */
  void Clear() { num  = 0;
	         lastrep = -1; }	/* Clear the list. */
 private:
  int num;			/* Number of superpositions. */
  int lastrep;			/* Last representation assigned */
  Superposition list[MAX_SUPERPOSITIONS]; /* List of superimposed specs. */
  friend class SuperpositionListIterator;
};
/*
** SuperpositionListIterator class definition:  The iterator allows clients
**   to flip through the list sequentially.  It also supports deletion of
**   the most recently retrieved element of the list.
*/
class SuperpositionListIterator {

 public:
  /* Constructors: */

  SuperpositionListIterator(SuperpositionList &l) {
    location = 0;
    slist     = &l;
  }
  /* Accessors etc. */

  Superposition &Next();	/* Return next superposition. */
  int            Last();	/* Is most recently retrieved the last one? */
  void DeleteCurrent();		/* Delete most recently retrieved one.      */

 private:
  int location;
  SuperpositionList *slist;

};
#endif
