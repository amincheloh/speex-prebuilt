const os = require('os')
const speex = require(`../built/${os.platform()}-speex`);

module.exports = speex;

module.exports.NARROW_BAND = 0;
module.exports.WIDE_BAND = 1;
module.exports.ULTRA_WIDE_BAND = 2;
