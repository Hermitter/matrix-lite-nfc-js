var nfc = require('./build/Release/addon');
var matrix = require('@matrix-io/matrix-lite');

// - Write To A Page (MIFARE Ultralight & NTAG)
// let page = nfc.page();

// setInterval(function(){
//     nfc.activate();

//     let message = "heyo".split("").map((x)=>{return x.charCodeAt(0);});
//     let code = page.write(21, message);//BE CAREFUL ON WHAT PAGE YOU WRITE TO!!!

//     nfc.deactivate();

//     if (code === 0){
//         console.log(nfc.status(code));
//     }
// }, 50);


// - Read A Page (MIFARE Ultralight & NTAG)
// let page = nfc.page();
// data = [];

// setInterval(function(){
//     nfc.activate();
//     data = page.read(1);
//     nfc.deactivate(); 

//     if (data.length > 0)
//         console.log(data);
// },0);

// - Reading Info (All tags)
// let info = nfc.info();
// let data = {};

// setInterval(function(){
//     nfc.activate();
//     data = info.read();
//     nfc.deactivate(); 

//     if (data.updated) {
//         console.log("********");
//         console.log(data);
//         matrix.led.set("green");
//     } else {
//         matrix.led.set("black");
//     }
// },50);
