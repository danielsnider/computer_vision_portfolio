// -----------------------------------------------------------------------------------
// http://wowslider.com/
// JavaScript Wow Slider is a free software that helps you easily generate delicious 
// slideshows with gorgeous transition effects, in a few clicks without writing a single line of code.
// Generated by WOW Slider 2.8
//
//***********************************************
// Obfuscated by Javascript Obfuscator
// http://javascript-source.com
//***********************************************
function ws_fade(c,a,b){var e=jQuery;var d=e("ul",b);var f={position:"absolute",left:0,top:0,width:"100%",height:"100%"};this.go=function(g,h){var i=e(a.get(g)).clone().css(f).hide().appendTo(b);if(!c.noCross){var j=e(a.get(h)).clone().css(f).appendTo(b);d.hide();j.fadeOut(c.duration,function(){j.remove()})}i.fadeIn(c.duration,function(){d.css({left:-g+"00%"}).show();i.remove()});return g}};// -----------------------------------------------------------------------------------
// http://wowslider.com/
// JavaScript Wow Slider is a free software that helps you easily generate delicious 
// slideshows with gorgeous transition effects, in a few clicks without writing a single line of code.
// Generated by WOW Slider 2.8
//
//***********************************************
// Obfuscated by Javascript Obfuscator
// http://javascript-source.com
//***********************************************
jQuery("#wowslider-container4").wowSlider({effect:"fade",prev:"",next:"",duration:15*100,delay:30*100,width:480,height:360,autoPlay:true,stopOnHover:false,loop:false,bullets:1,caption:false,captionEffect:"slide",controls:true,logo:"engine4/loading.gif",onBeforeStep:0,images:[{src:"./i1acropped.jpg",title:"i1a-cropped"},{src:"./i10aresultcsv.jpg",title:"i10a-result-csv"},{src:"./anki1.jpg",title:"anki1"},{src:"./anki2.jpg",title:"anki2"}]});