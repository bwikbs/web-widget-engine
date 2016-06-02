description("Test the parsing of the -webkit-backdrop-filter property.");

var styleElement = document.createElement("style");
document.head.appendChild(styleElement);
var stylesheet = styleElement.sheet;

// add a -webkit-filter property to the start of the stylesheet
stylesheet.addRule("body", "-webkit-backdrop-filter: hue-rotate(120deg)", 0);

var cssRule = stylesheet.cssRules.item(0);

shouldBe("cssRule.type", "1");

var declaration = cssRule.style;
shouldBe("declaration.length", "1");
shouldBe("declaration.getPropertyValue('-webkit-backdrop-filter')", "'hue-rotate(120deg)'");

successfullyParsed = true;
