
var page=require('webpage').create();
page.open('../../test/demo/clock/clock.html', function() {
    page.evaluate(function() {
        document.body.bgColor = 'white';
        document.body.style.fontFamily = 'Times New Roman';
    });
    console.log('clock');
    page.render('clock.png');
    phantom.exit();
} );
