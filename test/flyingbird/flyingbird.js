<?xml version="1.0" encoding="UTF-8"?>
<Document>
    <Image
        id = "bird"
        src = "test/flyingbird/bird.png"
        x = "130"
        y = "130"
        width = "100"
        height = "100"
    />
    <Text
        id = "record"
        textSize = "28"
        x = "0"
        y = "50"
        width = "330"
        textAlign = "right"
        text = "0.0"
    />
    <Element
        id = "knotBox"
        x = "0"
        y = "160"
        width = "360"
    >
        <Image
            id = "knot1"
            src = "test/flyingbird/1.png"
            x = "45"
            width = "50"
            height = "50"
        />
        <Image
            id = "knot2"
            src = "test/flyingbird/1.png"
            x = "100"
            width = "50"
            height = "50"
        />
        <Image
            id = "knot3"
            src = "test/flyingbird/1.png"
            x = "155"
            width = "50"
            height = "50"
        />
        <Image
            id = "knot4"
            src = "test/flyingbird/1.png"
            x = "210"
            width = "50"
            height = "50"
        />
        <Image
            id = "knot5"
            src = "test/flyingbird/1.png"
            x = "265"
            width = "50"
            height = "50"
        />

    </Element>
    <Text
        id = "gameOver"
        x = "40"
        y = "135"
        width = "0"
        height = "0"
        background = "rgba(0,150,220,255)"
        textAlign = "center"
        textSize = "42"
        text = ""
    />

  
<Script>
        <![CDATA[

        var NUMQUIZ = 5;
//        var NUMQUIZTYPE = 4;
        var NUMQUIZTYPE = 5;

        var quizBox = new Array(NUMQUIZ);
        var correctquiz;
        var quizarr = new Array(NUMQUIZ);
        var playingflag;
        var time_spent;
        var record;
        var bird_goal_height;
        var bird_current_height;

        var knotEnum = Object.freeze({counter_clockwise_down:0, clockwise_down:1,counter_clockwise_up:2,clockwise_up:3,touch:4});
     ///////////////////////////////////////////////////////


        function drawBird(redrawTime){
            if(!playingflag){
                document.getElementById("bird").x = "130";
                document.getElementById("bird").y = "130";
                
                return;
            }
            if(bird_goal_height < bird_current_height){
                if(bird_goal_height + 4 >= bird_current_height){
                    bird_goal_height = 500;
                } else{
                    bird_current_height = bird_current_height - (bird_current_height - bird_goal_height) / 4;
                    bird_current_height = bird_current_height | 0;
                    if(bird_current_height < 0){
                        bird_current_height = 0;
                    }
                    document.getElementById("bird").y = "" + bird_current_height;
                }

            } else{
                bird_current_height += 1;
                if(bird_current_height >= 260){
                    bird_current_height = 260;
                    gameOverEvent();
                } else {
                    document.getElementById("bird").y = "" + bird_current_height;
                }
            }
            window.setTimeout(function(){ drawBird(redrawTime) }, redrawTime * 1000);
        }
        function setBirdHeight(yCoord){
            bird_goal_height = yCoord;
        }

        function gameOverEvent(){
            playingflag = 0;
            gobtn.text = "Game Over";
            gobtn.width = "280";
            gobtn.height = "90";

        }
        function rotary_callback(direction){
            var thisknot = quizarr[correctquiz];
            if(direction == "cwise"){
                if(thisknot == knotEnum.clockwise_down || thisknot == knotEnum.clockwise_up){
                    correctquiz++;
                    document.getElementById("knot" + correctquiz).width = 25;
                    document.getElementById("knot" + correctquiz).height = 25;
                }
                else{
                    bird_current_height += 16;
                }
            } else if(direction == "ccwise"){
                if(thisknot == knotEnum.counter_clockwise_down || thisknot == knotEnum.counter_clockwise_up){
                    correctquiz++;
                    document.getElementById("knot" + correctquiz).width = 25;
                    document.getElementById("knot" + correctquiz).height = 25;
                }
                else{
                    bird_current_height += 16;
                }
            } else if(direction == "star"){
                if(thisknot == knotEnum.touch){
                    correctquiz++;
                    document.getElementById("knot" + correctquiz).width = 25;
                    document.getElementById("knot" + correctquiz).height = 25;
                }
                else{
                    bird_current_height += 16;
                }
            }

            if(correctquiz >= 5){
                knots_all_clear();
            } 
            
        }

        function knots_all_clear(){
            correctquiz = 0;
            setBirdHeight((bird_current_height - 500 / time_spent < 0) ? 0 : ((bird_current_height - 500 / time_spent) | 0));
            window.setTimeout(generate_random_knots, 3 * 1000);
        }

        function init(){
            playingflag = 1;
            record = 0.0;

            // hide buttons
            gobtn.text = "";
            gobtn.width = "0";
            gobtn.height = "0";
            

            // show elements
            document.getElementById("record").text = "0.0";

            bird_current_height = 130;
            bird_goal_height = 500;
            window.setTimeout(count_time_spent, 100);
            window.setTimeout(recordscore, 100);

            generate_random_knots();

            drawBird(0.03);
        }
        function generate_random_knots() {
            var i;
            correctquiz = 0;
            time_spent = 0.0;

            for(i = 0; i < NUMQUIZ; i++){
                quizarr[i] = Math.floor(Math.random() * NUMQUIZTYPE);
            }

            for(i = 0; i < NUMQUIZ; i++){
                quizBox[i] = document.getElementById("knot" + (i+1));
                quizBox[i].width = "50";
                quizBox[i].height = "50";
                quizBox[i].src = "test/flyingbird/" + quizarr[i] + ".png";
            }
        }

        function count_time_spent(){
            time_spent += 0.1;
            if(playingflag){
                window.setTimeout(count_time_spent, 100); // 0.1 * 1000
            }
        }

        function recordscore(){
            if(playingflag){
                record += 0.1;
                document.getElementById("record").text = "" + record.toFixed(1);
                window.setTimeout(recordscore, 100); // 0.1 * 1000
            }
        }


// start button

            // binding rotary event
            window.onKeyUp = function(e){
                if(e == "Right" || e == "rotaryClockWise"){
                    rotary_callback("cwise");
                }
                else if(e == "Left" || e == "rotaryCounterClockWise"){
                    rotary_callback("ccwise");
                }
                //TODO: Down -> screen ontouch event
                else if(e == "Down"){
                    rotary_callback("star");                
                }
            }

            var gobtn = document.getElementById("gameOver");
            gobtn.onClick = function(){
                init();
            }

        init();
       
        ]]>
    </Script>
</Document>
