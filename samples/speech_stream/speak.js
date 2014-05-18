var nodert_streams = require('nodert-streams');
var speech = require('windows.media.speechsynthesis');
var fs = require('fs');
var child_process = require('child_process');
var path = require('path');

var mediaPlayerPath = '"C:\\Program Files\\Windows Media Player\\wmplayer.exe"';
var mediaPlayerKillCmd = 'taskkill /IM wmplayer.exe';
var outputFilePath = path.join(__dirname, 'synthesized_text.wav');
var TIMEOUT = 5000;

var synth = new speech.SpeechSynthesizer();
synth.synthesizeTextToStreamAsync('Hello Node R T!', function(err, res) {
    if (err) {
        return console.info(err);
    }
    var inputStream = new nodert_streams.InputStream(res);
    var fileStream = fs.createWriteStream(outputFilePath);
    fileStream.on('close', function () {
        var child = child_process.exec(mediaPlayerPath + ' ' + outputFilePath);

        setTimeout(function () {
            child_process.exec(mediaPlayerKillCmd);
        }, TIMEOUT);
    });

    inputStream.pipe(fileStream);
});