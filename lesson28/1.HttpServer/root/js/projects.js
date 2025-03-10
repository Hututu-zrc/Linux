// 平滑滚动效果
	   document.querySelectorAll('a[href^="#"]').forEach(anchor => {
	       anchor.addEventListener('click', function(e) {
	           e.preventDefault();
	           document.querySelector(this.getAttribute('href')).scrollIntoView({
	               behavior: 'smooth'
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
	
	   // 切换项目架构的显示状态
	    function toggleCategoryVisibility(button) {
		   var project = button.closest('.project'); // 找到最近的父级项目
		   var category = project.querySelector('.category'); // 找到项目下的项目架构区域
   
		   if (category.style.display === 'none' || category.style.display === '') {
			   category.style.display = 'block'; // 展开项目架构
			   button.textContent = '收起项目架构';
		   } else {
			   category.style.display = 'none'; // 收起项目架构
			   button.textContent = '项目架构';
		   }
	   }