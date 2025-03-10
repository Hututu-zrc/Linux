const slide = document.querySelector('.carousel-slide');
    const images = document.querySelectorAll('.carousel-slide img');

    const prevBtn = document.getElementById('prevBtn');
    const nextBtn = document.getElementById('nextBtn');

    let counter = 0; 
    const size = images[0].clientWidth;

    slide.style.transform = 'translateX(' + (-size * counter) + 'px)';

    function nextSlide() {
        if (counter >= images.length - 1) return;
        slide.style.transition = 'transform 0.5s ease-in-out';
        counter++;
        slide.style.transform = 'translateX(' + (-size * counter) + 'px)';
        if (counter === images.length - 1) { 
            setTimeout(() => {
                slide.style.transition = 'none'; 
                counter = 0; 
                slide.style.transform = 'translateX(' + (-size * counter )+ 'px)';
            }, 0); 
        }
    }

    function prevSlide() {
        if (counter <= 0) return;
        slide.style.transition = 'transform 0.5s ease-in-out';
        counter--;
        slide.style.transform = 'translateX(' + (-size * counter) + 'px)';
    }

    nextBtn.addEventListener('click', nextSlide);
    prevBtn.addEventListener('click', prevSlide);

    setInterval(nextSlide, 2000);