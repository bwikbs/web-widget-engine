console.log(process.argv[2])

fs = require('fs')
fs.readFile(process.argv[2], 'utf8', function (err,data) {
   if (err) {
     return console.log(err);
   }
	
	var path = require('path')
	var absPath = path.resolve(__dirname , process.argv[2]);
	console.log(absPath)
	
	// convert
	var phantom = require('phantom');
	phantom.create(function (ph) {
  		ph.createPage(function (page) {
  			page.onConsoleMessage(function (msg) {
    			console.log("Phantom Console: " + msg)
			})
   		page.open("tool/html2xml/index.html", function (status) {
      		page.evaluate(function (data, absPath) {
      				var code = document.getElementById("code");
     					code.value = data;
     					onSubmit();
     					document.querySelector('iframe').src = absPath;
     					return "submit"
      			}, function (result) {
      				console.log(result);
      		}, data, absPath);
    		});
    		
    		
    		setTimeout(function wait() {
    			page.evaluate(function (absPath) {
      				var result = document.getElementById("result");
      				console.log(result.value)
      				
     					return result.value
      			}, function (result) {
      				ph.exit();
      				fs.writeFile('result.xml', result, function (err) {
  							if (err) throw err;
  							console.log('It\'s saved!');
							var d = path.dirname(absPath);
  							var child = require('child_process').execFile('./StarFish', ['result.xml' ,'--working-directory='+d + "/"])
  							child.stdout.on('data', function(buf) {
    							console.log('stdout "%s"', String(buf));
  							});
  							child.stderr.on('data', function(buf) {
    							console.log('stderr "%s"', String(buf));
  							});
							// child.stdout.pipe(process.stdout)
							child.on('exit', function() {
  								process.exit()
							})
  							
						});
      		}, absPath);
    			
    		}, 50)
  		});
	});
	
});