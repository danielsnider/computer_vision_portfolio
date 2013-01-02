
/*
 * POST upload file.
 */

// __dirname will be /Users/jojo/code/FaceBulge/routes
var facedetect_path = __dirname + '/../binaries/'
var exec = require('child_process').exec,
    child;
var fs = require("fs");
var path = require("path");

// maybe we could use async.js to avoid so much callback spaghetti
exports.action = function(req, res) {
  //child = exec('cd '+ facedetect_path + ' && ./a.out ' + req.files.file.path,
  process.chdir(facedetect_path);
  if (process.platform == "win32")
    command = '?.exe';
  else
    command = './quizit.out'; //actually this is a linux binary
  child = exec(command + ' "' + req.files.file.path + '"',
    function (error, stdout, stderr) {
      var photo_path = req.files.file.path.split(path.sep)
      res.render('uploaded', { title: 'QuizIt', face_info: stdout, photo_path: '/uploads/' + photo_path[photo_path.length - 1] });
      if (error !== null) {
        console.log('stdout: ' + stdout);
        console.log('stderr: ' + stderr);
        console.log('exec error: ' + error);
      }
  });
};

// maybe we could use async.js to avoid so much callback spaghetti
exports.contour_find = function(req, res) {
  //child = exec('cd '+ facedetect_path + ' && ./a.out ' + req.files.file.path,
  process.chdir(facedetect_path);
  if (process.platform == "win32")
	  command = '?.exe';
  else
	  command = './contour_find.out'; //actually this is a linux binary
    full_command = command + ' "' + req.files.file.path + '"' + ' ' + req.body.sep_dist + ' ' +  req.body.thres + ' ' + req.body.scan_rate;
  child = exec(full_command,
    function (error, stdout, stderr) {
      var result_photo = stdout;
      res.render('contour_find_uploaded', { title: 'Contour Find Result', photo_path: result_photo});
      if (error !== null) {
        console.log('stdout: ' + stdout);
        console.log('stderr: ' + stderr);
        console.log('exec error: ' + error);
      }
  });
};
