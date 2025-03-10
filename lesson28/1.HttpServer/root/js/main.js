document.addEventListener('DOMContentLoaded', () => {
    const navLinks = document.querySelectorAll('nav ul li a');
    const pages = document.querySelectorAll('.page');
    
    navLinks.forEach(link => {
        link.addEventListener('click', (e) => {
            e.preventDefault();
            const page = e.target.getAttribute('data-page');
            pages.forEach(p => p.classList.remove('active'));
            document.getElementById(page).classList.add('active');
            window.scrollTo(0, 0); // 切换页面时滚动到顶部
        });
    });

    loadProjects();
    loadBlogPosts();

    // 表单验证
    document.getElementById('contact-form').addEventListener('submit', function(event) {
        event.preventDefault();
        const name = document.getElementById('name').value;
        const email = document.getElementById('email').value;
        const message = document.getElementById('message').value;
        if (name && email && message) {
            alert('信息已提交！');
            this.reset(); // 提交后重置表单
        } else {
            alert('请填写所有字段。');
        }
    });
});

