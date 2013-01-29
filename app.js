
/**
 * Module dependencies.
 */

var express = require('express')
  , routes = require('./routes')
  , restarter = require('./routes/restart')
  , upload = require('./routes/upload')
  , http = require('http')
  , path = require('path');

var app = express();



app.configure(function(){
  app.set('port', process.env.PORT || 3000);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'ejs');
  app.use(express.favicon());
  app.use(express.logger('dev'));
  app.use(express.bodyParser({keepExtensions: true, uploadDir: __dirname + "/public/uploads"}));
  app.use(express.methodOverride());
  app.use(app.router);
  app.use(express.static(path.join(__dirname, 'public')));
});

app.configure('development', function(){
  app.use(express.errorHandler());
});

app.get('/', routes.index);
app.get('/index.html', routes.indexhtml);
app.post('/', upload.action);
app.post('/contour_find', upload.contour_find);
app.post('/quiz_it', upload.quiz_it);
app.post('/empty_wall_space', upload.empty_wall_space);
app.post('/highlight_ocr', upload.highlight_ocr);


app.post('/restart', restarter.index);

http.createServer(app).listen(app.get('port'), function(){
  console.log("Express server listening on port " + app.get('port'));
});

//to access any file in tree from browser
//examle: http://192.168.71.153:8080/views/index.html
//npm install connect
var connect = require('connect');

connect.createServer(
    connect.static(__dirname)
).listen(8080);
