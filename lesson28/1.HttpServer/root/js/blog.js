document.addEventListener('DOMContentLoaded', function() {
	    const categoryButtons = document.querySelectorAll('.category-button');
	    const cards = document.querySelectorAll('.card');
	    const blogPosts = document.getElementById('blog-posts');
	
	    categoryButtons.forEach(button => {
	        button.addEventListener('click', function() {
	            const category = this.getAttribute('data-category');
	
	            cards.forEach(card => {
	                const cardCategory = card.classList[1]; // 获取卡片的分类类名
	
	                if (category === 'all' || cardCategory === category) {
	                    card.style.display = 'block';
	                    blogPosts.insertBefore(card, blogPosts.firstChild); // 将符合条件的卡片移到首位
	                    setTimeout(() => {
	                        card.classList.add('deal-animation'); // 添加旋转动画类
	                    }, 50); // 延迟一小段时间以确保动画效果
	                } else {
	                    card.style.display = 'none';
	                    card.classList.remove('deal-animation'); // 移除旋转动画类
	                }
	            });
	        });
	    });
	});
	
	// 返回顶部按钮
	var backToTopButton = document.getElementById("back-to-top");
	window.onscroll = function() {
	    if (document.body.scrollTop > 200 || document.documentElement.scrollTop > 200) {
	        backToTopButton.style.display = "block";
	    } else {
	        backToTopButton.style.display = "none";
	    }
	};
	backToTopButton.addEventListener('click', function() {
	    window.scrollTo({top: 0, behavior: 'smooth'});
	});
