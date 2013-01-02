
/*
 * GET home page.
 */

exports.index = function(req, res){
  res.render('index', { title: 'Computer Vision Portfolio by DS' });
};