﻿description("Test URL protocol setter.");

var a = document.createElement("a");
a.setAttribute("href", "http://www.apple.com/");
document.body.appendChild(a);

shouldThrow("location.protocol = ''", '"SyntaxError: Failed to set the \'protocol\' property on \'Location\': \'\' is an invalid protocol."');
shouldThrow("location.protocol = ':'", '"SyntaxError: Failed to set the \'protocol\' property on \'Location\': \':\' is an invalid protocol."');
shouldThrow("location.protocol = 'é'", '"SyntaxError: Failed to set the \'protocol\' property on \'Location\': \'é\' is an invalid protocol."');
shouldThrow("location.protocol = '['", '"SyntaxError: Failed to set the \'protocol\' property on \'Location\': \'[\' is an invalid protocol."');
shouldThrow("location.protocol = '0'", '"SyntaxError: Failed to set the \'protocol\' property on \'Location\': \'0\' is an invalid protocol."');

// IE raises exceptions for anchors, too - but Firefox does not. In either case, protocol shouldn't change.
try { a.protocol = '' } catch (ex) { }
try { a.protocol = 'é' } catch (ex) { }
try { a.protocol = '[' } catch (ex) { }
try { a.protocol = '0' } catch (ex) { }
shouldBe("a.protocol", "'http:'");

a.protocol = "https";
shouldBe("a.href", "'https://www.apple.com/'");

a.protocol = "http:";
shouldBe("a.href", "'http://www.apple.com/'");

a.protocol = "https://foobar";
shouldBe("a.href", "'https://www.apple.com/'");
