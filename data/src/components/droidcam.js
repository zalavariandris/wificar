/*
Set video feed URL from query parameters on load
*/

function parseURI(){
    let query  = window.location.search.substring(1);

    // parse uri to json
    let match;
    let pl     = /\+/g;  // Regex for replacing addition symbol with a space
    let search = /([^&=]+)=?([^&]*)/g;
    let decode = function (s) { 
        return decodeURIComponent(s.replace(pl, " "));
    };

    let urlParams = {};
    while (match = search.exec(query)){
       urlParams[decode(match[1])] = decode(match[2]);
    }

      return urlParams;
}

class Droidcam{
    constructor({selector, ip}){
        // setup
        this.element = document.querySelector(selector);
        this._img = this.element.querySelector("img")
        this._info = this.element.querySelector(".info")
        

        // set IP
        const video_url = "http://"+ip+"/video"

        // update
        this._img.setAttribute("src", video_url)
        this._info.innerText = `droidcam ip: ${ip}`

    }
}