
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
      return "http://www.w3.org/2001/DOM-Test-Suite/level2/core/normalize01";
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
    The "normalize()" method puts all the nodes in the full
    depth of the sub-tree underneath this element into a 
    "normal" form. 
    
    Retrieve the third employee and access its second child.
    This child contains a block of text that is spread 
    across multiple lines.   The content of the "name" child
    should be parsed and treated as a single Text node.

    This appears to be a duplicate of elementnormalize.xml in DOM L1 Test Suite

* @author NIST
* @author Mary Brady
* @see http://www.w3.org/TR/DOM-Level-2-Core/core#ID-normalize
* @see http://www.w3.org/TR/DOM-Level-2-Core/core#ID-72AB8359
*/
function normalize01() {
   var success;
    if(checkInitialization(builder, "normalize01") != null) return;
    var doc;
      var root;
      var elementList;
      var firstChild;
      var textList;
      var textNode;
      var data;
      
      var docRef = null;
      if (typeof(this.doc) != 'undefined') {
        docRef = this.doc;
      }
      doc = load(docRef, "doc", "hc_staff");
      root = doc.documentElement;

      root.normalize();
      elementList = root.getElementsByTagName("name");
      firstChild = elementList.item(2);
      textList = firstChild.childNodes;

      textNode = textList.item(0);
      data = textNode.data;

      assertEquals("data","Roger\n Jones",data);
       
}




function runTest() {
   normalize01();
}
