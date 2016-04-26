
/*
Copyright Â© 2001-2004 World Wide Web Consortium, 
(Massachusetts Institute of Technology, European Research Consortium 
for Informatics and Mathematics, Keio University). All 
Rights Reserved. This work is distributed under the W3CÂ® Software License [1] in the 
hope that it will be useful, but WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

[1] http://www.w3.org/Consortium/Legal/2002/copyright-software-20021231
*/



   /**
    *  Gets URI that identifies the test.
    *  @return uri identifier of test
    */
function getTargetURI() {
      return "http://www.w3.org/2001/DOM-Test-Suite/level1/core/attrsetvaluenomodificationallowederr";
   }

var docsLoaded = -1000000;
var builder = null;

//
//   This function is called by the testing framework before
//      running the test suite.
//
//   If there are no configuration exceptions, asynchronous
//        document loading is started.  Otherwise, the status
//        is set to complete and the exception is immediately
//        raised when entering the body of the test.
//
function setUpPage() {
   setUpPageStatus = 'running';
   try {
     //
     //   creates test document builder, may throw exception
     //
     builder = createConfiguredBuilder();
       setImplementationAttribute("expandEntityReferences", false);

      docsLoaded = 0;
      
      var docRef = null;
      if (typeof(this.doc) != 'undefined') {
        docRef = this.doc;
      }
      docsLoaded += preload(docRef, "doc", "hc_staff");
        
       if (docsLoaded == 1) {
          setUpPageStatus = 'complete';
       }
    } catch(ex) {
    	catchInitializationError(builder, ex);
        setUpPageStatus = 'complete';
    }
}



//
//   This method is called on the completion of 
//      each asychronous load started in setUpTests.
//
//   When every synchronous loaded document has completed,
//      the page status is changed which allows the
//      body of the test to be executed.
function loadComplete() {
    if (++docsLoaded == 1) {
        setUpPageStatus = 'complete';
    }
}


/**
* 
    The "setValue()" method for an attribute causes the 
  DOMException NO_MODIFICATION_ALLOWED_ERR to be raised
  if the node is readonly.
  Obtain the children of the THIRD "gender" element.  The elements
  content is an entity reference.  Get the "domestic" attribute
  from the entity reference and execute the "setValue()" method.
  This causes a NO_MODIFICATION_ALLOWED_ERR DOMException to be thrown.

* @author NIST
* @author Mary Brady
* @see http://www.w3.org/TR/2000/WD-DOM-Level-1-20000929/level-one-core#xpointer(id('ID-258A00AF')/constant[@name='NO_MODIFICATION_ALLOWED_ERR'])
* @see http://www.w3.org/TR/2000/WD-DOM-Level-1-20000929/level-one-core#ID-221662474
* @see http://www.w3.org/TR/2000/WD-DOM-Level-1-20000929/level-one-core#xpointer(id('ID-221662474')/setraises/exception[@name='DOMException']/descr/p[substring-before(.,':')='NO_MODIFICATION_ALLOWED_ERR'])
* @see http://www.w3.org/DOM/updates/REC-DOM-Level-1-19981001-errata.html
* @see http://www.w3.org/TR/1998/REC-DOM-Level-1-19981001/level-one-core#ID-221662474
*/
function attrsetvaluenomodificationallowederr() {
   var success;
    if(checkInitialization(builder, "attrsetvaluenomodificationallowederr") != null) return;
    var doc;
      var genderList;
      var gender;
      var genList;
      var gen;
      var gList;
      var g;
      var attrList;
      var attrNode;
      
      var docRef = null;
      if (typeof(this.doc) != 'undefined') {
        docRef = this.doc;
      }
      doc = load(docRef, "doc", "hc_staff");
      genderList = doc.getElementsByTagName("gender");
      gender = genderList.item(2);
      assertNotNull("genderNotNull",gender);
genList = gender.childNodes;

      gen = genList.item(0);
      assertNotNull("genderFirstChildNotNull",gen);
gList = gen.childNodes;

      g = gList.item(0);
      assertNotNull("genderFirstGrandchildNotNull",g);
attrList = g.attributes;

      assertNotNull("attributesNotNull",attrList);
attrNode = attrList.getNamedItem("domestic");
      assertNotNull("attrNotNull",attrNode);

	{
		success = false;
		try {
            attrNode.value = "newvalue";

        }
		catch(ex) {
      success = (typeof(ex.code) != 'undefined' && ex.code == 7);
		}
		assertTrue("setValue_throws_NO_MODIFICATION",success);
	}

	{
		success = false;
		try {
            attrNode.nodeValue = "newvalue2";

        }
		catch(ex) {
      success = (typeof(ex.code) != 'undefined' && ex.code == 7);
		}
		assertTrue("setNodeValue_throws_NO_MODIFICATION",success);
	}

}




function runTest() {
   attrsetvaluenomodificationallowederr();
}
