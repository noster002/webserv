<script src="	https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha3/dist/js/bootstrap.bundle.min.js"></script>
<script src="https://code.jquery.com/jquery-3.5.1.js"></script>
<script src="https://cdn.datatables.net/1.13.4/js/jquery.dataTables.min.js"></script>
<script src="https://cdn.datatables.net/1.13.4/js/dataTables.bootstrap5.min.js"></script>
<script>
	function put_request(e)
	{
		alert("Sorry ! we're lazy guys, so delete and add a new one !");
	}

	function del_request(e)
	{
		if (confirm("Are you sure to delete your profile image ?"))
		{
			const res = e.id;
			const base = "http://localhost:8082/";
			const url = base + res;
			fetch(url, {
				method: 'DELETE'
			})
			.then(res => {
				alert("Your profile image sucessfully deleted!");
				location.reload();
			})
			.catch(error => {
				console.log("Request failed:", error);
			})

		}
	}

	$(document).ready(function () {
		$('#example').DataTable();
	});
</script>
</body>
</html>