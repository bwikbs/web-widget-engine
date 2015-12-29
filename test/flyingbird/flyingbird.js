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
  
<Script>
        <![CDATA[

        var NUMQUIZ = 5;
        var NUMQUIZTYPE = 4;
//        var NUMQUIZTYPE = 5;

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
print("" + bird_current_height);                    
                    document.getElementById("bird").y = "" + bird_current_height;
                }

            } else{
                bird_current_height += 2;
                if(bird_current_height >= 260){
                    bird_current_height = 260;
//                    gameOverEvent();
                }
                document.getElementById("bird").y = "" + bird_current_height;
            }
            window.setTimeout(function(){ drawBird(redrawTime) }, redrawTime * 1000);
        }
        function setBirdHeight(yCoord){
            bird_goal_height = yCoord;
        }

        function rotary_callback(direction){
            var thisknot = quizarr[correctquiz];
            print(thisknot);
            print(direction);
            if(direction == "cwise"){
                if(thisknot == knotEnum.clockwise_down || thisknot == knotEnum.clockwise_up){
                    correctquiz++;
                    document.getElementById("knot" + correctquiz).width = 25;
                    document.getElementById("knot" + correctquiz).height = 25;
                    print(correctquiz);
                }
                else{
                    bird_current_height += 32;
                }
            } else if(direction == "ccwise"){
                if(thisknot == knotEnum.counter_clockwise_down || thisknot == knotEnum.counter_clockwise_up){
                    correctquiz++;
                    document.getElementById("knot" + correctquiz).width = 25;
                    document.getElementById("knot" + correctquiz).height = 25;
                    print(correctquiz);
                }
                else{
                    bird_current_height += 32;
                }
            } else if(direction == "star"){
                if(thisknot == knotEnum.touch){
                    correctquiz++;
                    document.getElementById("knot" + correctquiz).width = 25;
                    document.getElementById("knot" + correctquiz).height = 25;
                   print(correctquiz);
                }
                else{
                    bird_current_height += 32;
                }
            }

            if(correctquiz >= 5){
                knots_all_clear();
            } 
            
        }

        function knots_all_clear(){
            correctquiz = 0;
            setBirdHeight((bird_current_height - 500 / time_spent < 0) ? 0 : (bird_current_height - 500 / time_spent));
        }

        function init(){
            playingflag = 1;
            record = 0.0;

            // hide buttons

            bird_current_height = 130;
            bird_goal_height = 500;

            generate_random_knots();

            drawBird(0.1);
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
       print(quizBox[i]);
                quizBox[i].src = "test/flyingbird/" + quizarr[i] + ".png";
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

            // make and start timers

            // draw bird
            // gameover button

        init();
       
        ]]>
    </Script>
</Document>
