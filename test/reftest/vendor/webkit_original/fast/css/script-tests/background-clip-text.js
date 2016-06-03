description("This tests checks that the '-webkit-text' and 'text' keywords are \
    parsed correctly in the 'background-clip' and '-webkit-background-clip' \
    properties, and that 'background-clip' is parsed correctly in the \
    'background' shorthand.");

function test(declaration, property)
{
    var div = document.createElement("div");
    div.setAttribute("style", declaration);
    document.body.appendChild(div);
    
    var result = div.style.getPropertyValue(property);
    document.body.removeChild(div);
    return result;
}
shouldBe('test("background-clip: -webkit-text", "background-clip")', '"-webkit-text"');
shouldBeEqualToString('test("background-clip: -webkit-text", "-webkit-background-clip")', '');

shouldBe('test("background-clip: content-box", "background-clip")', '"content-box"');
shouldBe('test("background-clip: padding-box", "background-clip")', '"padding-box"');
shouldBe('test("background-clip: border-box", "background-clip")', '"border-box"');

shouldBeEqualToString('test("background-clip: text", "background-clip")', '');
shouldBeEqualToString('test("background-clip: text", "-webkit-background-clip")', '');

shouldBeEqualToString('test("-webkit-background-clip: -webkit-text", "background-clip")', '');
shouldBe('test("-webkit-background-clip: -webkit-text", "-webkit-background-clip")', '"-webkit-text"');

shouldBeEqualToString('test("-webkit-background-clip: text", "background-clip")', '');
shouldBe('test("-webkit-background-clip: text", "-webkit-background-clip")', '"text"');
shouldBeEqualToString('test("background: url() padding-box", "-webkit-background-clip")', '');

shouldBeEqualToString('test("-webkit-mask: url() ", "background-clip")', '');
shouldBe('test("background: url() ", "background-clip")', '"initial"');

shouldBe('test("background: url() padding-box", "background-clip")', '"padding-box"');
shouldBe('test("background: url() padding-box border-box", "background-clip")', '"border-box"');
shouldBe('test("background: repeat content-box border-box url() ", "background-clip")', '"border-box"');
shouldBe('test("background: padding-box blue content-box url() repeat scroll ", "background-clip")', '"content-box"');
