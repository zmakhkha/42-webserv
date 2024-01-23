const tl = gsap.timeline({default : {ease : 'power2.out'}});

tl.to(".header", {opacity: "1", duration : 1});
tl.to(".pictures", {y : "0%", opacity: "1", duration : 1});

const conf = document.getElementById('pic_m');
const conf_z = document.getElementById('pic_z');
const conf_m = document.getElementById('pic_m_2');

conf_m.addEventListener('mouseover', () => {
    conf_m.innerHTML = "a9wad config file parser 2";
    conf_m.style.background = "black";
});
conf_m.addEventListener('mouseleave', () => {
    conf_m.innerHTML = "";
    conf_m.style.background = "url('https://cdn.intra.42.fr/users/29a8a6aa878cdfaa350b73c855aab2cd/mnassi.jpg')";
    conf_m.style.backgroundSize = "cover";
    conf_m.style.backgroundPosition = "0px -10px";
});

conf_z.addEventListener('mouseover', () => {
    conf_z.innerHTML = "hada zmakhkha o safi.";
    conf_z.style.background = "black";
});
conf_z.addEventListener('mouseleave', () => {
    conf_z.innerHTML = "";
    conf_z.style.background = "url('./assets/d.jpeg')";
    conf_z.style.backgroundSize = "cover";
    conf_z.style.backgroundPosition = "0px";
});
conf_m.addEventListener('mouseover', () => {
    conf_m.innerHTML = "a9wed wahed f lm9wdin elihom fl web";
    conf_m.style.background = "black";
});
conf_m.addEventListener('mouseleave', () => {
    conf_m.innerHTML = "";
    conf_m.style.background = "url('https://cdn.intra.42.fr/users/29a8a6aa878cdfaa350b73c855aab2cd/mnassi.jpg')";
    conf_m.style.backgroundSize = "cover";
    conf_m.style.backgroundPosition = "0px -10px";
});
