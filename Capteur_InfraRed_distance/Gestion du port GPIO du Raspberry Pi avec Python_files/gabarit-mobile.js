$(function() {
	$('#gabarit_mobile span').click(gabaritMobile_ouvrirMenu);
	$('#gabarit_menu_mobile div').click(gabaritMobile_fermerMenu);
	$(window).resize(gabaritMobile_fermerMenu);
});

function gabaritMobile_ouvrirMenu() {
	$('#gabarit_menu_mobile p span:first').show();
	$('#gabarit_menu_mobile p span:last').hide();
	$('#gabarit_menu_mobile ul').html($('#gabarit_menu').html());
	$('#gabarit_menu_mobile a').click(gabaritMobile_clicRubrique);
	$('#gabarit_menu_mobile ul .gabarit_selection').removeClass('gabarit_selection');
	$('#gabarit_menu_mobile').show();
}

function gabaritMobile_fermerMenu() {
	$('#gabarit_menu_mobile').hide();
}

function gabaritMobile_clicRubrique() {
	var elem = $(this);

	if (elem.attr('data-idrub') == 1 || elem.attr('data-niv') == 2)
		return true;

	$('#gabarit_menu_mobile p span:first').hide();
	$('#gabarit_menu_mobile p span:last').show();
	$('#gabarit_menu_mobile ul').html(elem.parent().children('ul').html());
	$('#gabarit_menu_mobile a').click(gabaritMobile_clicRubrique);

	return false;
}
