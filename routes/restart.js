
/*
 * GET home page.
 */

exports.index = function(req, res){
  github_ips = ["207.97.227.253", "50.57.128.197", "108.171.174.178"]
  if (github_ips.indexOf(req.connection.remoteAddress) > -1) {
    console.log("deploying!")
    console.log("shipping mode: ENABLE! http://i.imgur.com/SrAsR.jpg")
    process.exit()
  }
};