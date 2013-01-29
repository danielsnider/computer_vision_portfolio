
/*
 * POST upload file.
 */

// __dirname will be /Users/jojo/code/FaceBulge/routes
var binaries_path = __dirname + '/../binaries/';
var exec = require('child_process').exec, child;
var fs = require("fs");
var path = require("path");

// maybe we could use async.js to avoid so much callback spaghetti
exports.action = function(req, res) {
  //child = exec('cd '+ binaries_path + ' && ./a.out ' + req.files.file.path,
  process.chdir(binaries_path);
  if (process.platform == "win32")
    command = '?.exe';
  else
    command = './quizit.out'; //actually this is a linux binary
  child = exec(command + ' "' + req.files.file.path + '"',
    function (error, stdout, stderr) {
      var photo_path = req.files.file.path.split(path.sep);
      res.render('uploaded', { title: 'QuizIt', face_info: stdout, photo_path: '/uploads/' + photo_path[photo_path.length - 1] });
      if (error !== null) {
        console.log('stdout: ' + stdout);
        console.log('stderr: ' + stderr);
        console.log('exec error: ' + error);
      }
  });
};

exports.contour_find = function(req, res) {
  process.chdir(binaries_path);

  var orig_file_name = req.files.file.name;
  var new_file_path = req.files.file.path.split(path.sep); //array of components of filename
  var new_file_name = new_file_path[new_file_path.length - 1]; //just the photo name

  var input_photo = "../public/uploads/" + new_file_name;
  var output_photo = "../public/uploads/" + new_file_name + '-contour_find-processed.jpg';
  var display_photo = "/uploads/" + new_file_name + '-contour_find-processed.jpg';
  var orig_display_photo = "/uploads/" + new_file_name;

  if (orig_file_name == '')
  {
    input_photo = "../public/images/contour_find_original.JPG";
    orig_display_photo = "/images/contour_find_original.JPG";
  }

  command = './contour_find.out' + ' "' + input_photo + '" ' + req.body.sep_dist + ' ' +  req.body.thres + ' ' + req.body.scan_rate + ' "'+ output_photo + '"';
  child = exec(command,
    function (error, stdout, stderr) {
      res.render('contour_find_uploaded', { title: 'Contour Find', input_photo_path: orig_display_photo, photo_path: display_photo});
      if (error !== null) {
        console.log('stdout: ' + stdout);
        console.log('stderr: ' + stderr);
        console.log('exec error: ' + error);
      }
  });
};

exports.quiz_it = function(req, res) {
  process.chdir(binaries_path);

  var orig_file_name = req.files.file.name;
  var new_file_path = req.files.file.path.split(path.sep); //array of components of filename
  var new_file_name = new_file_path[new_file_path.length - 1]; //just the photo name

  var input_photo = "../public/uploads/" + new_file_name;
  var orig_display_photo = "/uploads/" + new_file_name;

  if (orig_file_name == '')
  {
    input_photo = "../public/images/quiz_it.png";
    orig_display_photo = "/images/quiz_it.png";
  }

  command = './quiz_it.out' + ' "' + input_photo + '"';
  child = exec(command,
    function (error, stdout, stderr) {
      var result_text = stdout;
      res.render('quiz_it_uploaded', { title: 'Quiz It', input_photo_path: orig_display_photo, OCR_text: result_text});
      if (error !== null) {
        console.log('stdout: ' + stdout);
        console.log('stderr: ' + stderr);
        console.log('exec error: ' + error);
      }
  });
};

exports.empty_wall_space = function(req, res) {
  process.chdir(binaries_path);

  var orig_file_name = req.files.file.name;
  var new_file_path = req.files.file.path.split(path.sep); //array of components of filename
  var new_file_name = new_file_path[new_file_path.length - 1]; //just the photo name

  var input_photo = "../public/uploads/" + new_file_name;
  var output_photo = "../public/uploads/" + new_file_name + '-empty_wall_space-processed.jpg';
  var display_photo = "/uploads/" + new_file_name + '-empty_wall_space-processed.jpg';
  var orig_display_photo = "/uploads/" + new_file_name;

  if (orig_file_name == '')
  {
    input_photo = "../public/images/empty_wall_space.jpg";
    orig_display_photo = "/images/empty_wall_space.jpg";
  }

  command = './empty_wall_space.out' + ' "' + input_photo + '" "' + output_photo + '"';
  child = exec(command,
    function (error, stdout, stderr) {
      res.render('empty_wall_space_uploaded', { title: 'Empty Wall Space', input_photo_path: orig_display_photo, photo_path: display_photo});
      if (error !== null) {
        console.log('stdout: ' + stdout);
        console.log('stderr: ' + stderr);
        console.log('exec error: ' + error);
      }
  });
};

exports.highlight_ocr = function(req, res) {
  process.chdir(binaries_path);

  var orig_file_name = req.files.file.name;
  var new_file_path = req.files.file.path.split(path.sep); //array of components of filename
  var new_file_name = new_file_path[new_file_path.length - 1]; //just the photo name

  var input_photo = "../public/uploads/" + new_file_name;
  var orig_display_photo = "/uploads/" + new_file_name;

  if (orig_file_name == '')
  {
    input_photo = "../public/images/highlight_ocr.png";
    orig_display_photo = "/images/highlight_ocr.png";
  }

  command = './highlight_ocr.out' + ' "' + input_photo + '" ' + req.body.red + ' ' + req.body.green + ' ' + req.body.blue;
  child = exec(command,
    function (error, stdout, stderr) {
      var result_text = stdout;
      res.render('highlight_ocr_uploaded', { title: 'Highlights only OCR', input_photo_path: orig_display_photo, OCR_text: result_text});
      if (error !== null) {
        console.log('stdout: ' + stdout);
        console.log('stderr: ' + stderr);
        console.log('exec error: ' + error);
      }
  });
};
